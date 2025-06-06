#include "ivu_types.hxx"

inline void hk_VecFPU::fpu_add_multiple_row(hk_real *target_adress,hk_real *source_adress,hk_real factor,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		//we have to calculate the block size and shift adresses to lower aligned adresses
		hk_intp result_adress = reinterpret_cast<hk_intp>(source_adress) & hk_VecFPU_MEM_MASK_FLOAT;

		target_adress = reinterpret_cast<hk_real*>( reinterpret_cast<hk_intp>(target_adress) & hk_VecFPU_MEM_MASK_FLOAT);
		size += (reinterpret_cast<hk_intp>(source_adress)-result_adress)>>hk_VecFPU_MEMSHIFT_FLOAT;
		source_adress=reinterpret_cast<hk_real*>(result_adress);
    }

#if 0
#if defined(IVP_WILLAMETTE)
    __m128d factor128=_mm_set1_pd(factor);

    for(int i=size;i>0;i-=hk_VecFPU_SIZE_FLOAT) {
	    __m128d source_d=_mm_load_pd(source_adress);
	    __m128d prod_d=_mm_mul_pd(factor128,source_d);
	    __m128d target_d=_mm_load_pd(target_adress);
	    target_d=_mm_add_pd(prod_d,target_d);
	    _mm_store_pd(target_adress,target_d);

	    target_adress+=hk_VecFPU_SIZE_FLOAT;
	    source_adress+=hk_VecFPU_SIZE_FLOAT;
	}
#endif
#endif

    for(int i=size;i>0;i-=hk_VecFPU_SIZE_FLOAT) {
#if hk_VecFPU_SIZE_FLOAT == 2
		hk_real a = source_adress[0] * factor;
		hk_real b = source_adress[1] * factor;
		a += target_adress[0];
		b += target_adress[1];
		target_adress[0] = a;
		target_adress[1] = b;
#elif hk_VecFPU_SIZE_FLOAT == 4
		hk_real a = source_adress[0] * factor;
		hk_real b = source_adress[1] * factor;
		hk_real c = source_adress[2] * factor;
		hk_real d = source_adress[3] * factor;
		a += target_adress[0];
		b += target_adress[1];
		c += target_adress[2];
		d += target_adress[3];
		target_adress[0] = a;
		target_adress[1] = b;
		target_adress[2] = c;
		target_adress[3] = d;
#else 
#error Unknown float size
#endif

		target_adress+=hk_VecFPU_SIZE_FLOAT;
		source_adress+=hk_VecFPU_SIZE_FLOAT;
	}
}

inline hk_real hk_VecFPU::fpu_large_dot_product(hk_real *base_a, hk_real *base_b, int size, hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
	    //we have to calculate the block size and shift adresses to lower aligned adresses
	    hk_intp result_adress = reinterpret_cast<hk_intp>(base_a) & hk_VecFPU_MEM_MASK_FLOAT;
	    base_b = reinterpret_cast<hk_real*>( reinterpret_cast<hk_intp>(base_b) & hk_VecFPU_MEM_MASK_FLOAT);
	    size += (reinterpret_cast<hk_intp>(base_a)-result_adress)>>hk_VecFPU_MEMSHIFT_FLOAT;  // because start changed
	    base_a=reinterpret_cast<hk_real*>(result_adress);
    }

#if defined(IVP_WILLAMETTE)
    IVP_IF_WILLAMETTE_OPT(IVP_Environment_Manager::get_environment_manager()->ivp_willamette_optimization) {

	__m128d sum128 =_mm_set1_pd(0.0f);

	int i;
	for(i=size;i>=hk_VecFPU_SIZE_FLOAT; i-= hk_VecFPU_SIZE_FLOAT) {
	    __m128d mult1=_mm_load_pd(base_a);
	    __m128d mult2=_mm_load_pd(base_b);
	    __m128d prod =_mm_mul_pd(mult1,mult2);
	    sum128 =_mm_add_pd(prod,sum128);
	    base_a += hk_VecFPU_SIZE_FLOAT;
	    base_b += hk_VecFPU_SIZE_FLOAT;
	}

	__m128d dummy,low,high;
	low=sum128;
	dummy=sum128;
	//_mm_shuffle_pd(sum128,sum128,1); //swap high and low
	sum128=_mm_unpackhi_pd(sum128,dummy);
	high=sum128;
	__m128d sum64=_mm_add_sd(low,high);
	__m128d res=sum64;
	//_mm_shuffle_pd(sum64,sum64,1); 
	hk_real result_sum; 
	_mm_store_sd(&result_sum,res);
	
	for(;i>=0;i--) {
	    result_sum += base_a[i] * base_b[i];
	}

	return result_sum;
    } else {
	hk_real sum=0.0f;
	for(int i=size-1;i>=0;i--) {
	    sum += base_a[i] * base_b[i];
	}
	return sum;
    } 
#else
    hk_real sum=0.0f;
    for(int i=0;i<size;i++) {
		sum += base_a[i] * base_b[i];
    }
    return sum;
#endif
}

inline void hk_VecFPU::fpu_multiply_row(hk_real *target_adress,hk_real factor,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		//we have to calculate the block size and shift adresses to lower aligned adresses
		hk_intp adress,result_adress;
		adress=reinterpret_cast<hk_intp>(target_adress);
		result_adress=adress & hk_VecFPU_MEM_MASK_FLOAT;
		size+=(adress-result_adress)>>hk_VecFPU_MEMSHIFT_FLOAT;
		target_adress=reinterpret_cast<hk_real*>(result_adress);
    }

#if 0
#ifdef IVP_WILLAMETTE
    __m128d factor128=_mm_set1_pd(factor);
    for(int i=size;i>0;i-=hk_VecFPU_SIZE_FLOAT) {
		__m128d target_d=_mm_load_pd(target_adress);
		target_d=_mm_mul_pd(factor128,target_d);
		_mm_store_pd(target_adress,target_d);
	
		target_adress+=hk_VecFPU_SIZE_FLOAT;
	}    
#endif
#endif

    for(int i=size;i>0;i-=hk_VecFPU_SIZE_FLOAT) {
#if hk_VecFPU_SIZE_FLOAT == 2
	    hk_real a = target_adress[0] * factor;
	    hk_real b = target_adress[1] * factor;
	    target_adress[0] = a;
	    target_adress[1] = b;
#elif hk_VecFPU_SIZE_FLOAT == 4
	    hk_real a = target_adress[0] * factor;
	    hk_real b = target_adress[1] * factor;
	    hk_real c = target_adress[2] * factor;
	    hk_real d = target_adress[3] * factor;
	    target_adress[0] = a;
	    target_adress[1] = b;
	    target_adress[2] = c;
	    target_adress[3] = d;
#else
#error Unknown float size
#endif

		target_adress+=hk_VecFPU_SIZE_FLOAT;
	}
}

// #+# sparc says rui, optimize for non vector units ( hk_VecFPU_SIZE_FLOAT = 4 )
inline void hk_VecFPU::fpu_exchange_rows(hk_real *target_adress1,hk_real *target_adress2,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		//we have to calculate the block size and shift adresses to lower aligned adresses
		hk_intp adress=reinterpret_cast<hk_intp>(target_adress1);
		hk_intp result_adress=adress & hk_VecFPU_MEM_MASK_FLOAT;
		size+=(adress-result_adress)>>hk_VecFPU_MEMSHIFT_FLOAT;
		target_adress1=reinterpret_cast<hk_real*>(result_adress);
		adress=reinterpret_cast<hk_intp>(target_adress2);
		adress=adress & hk_VecFPU_MEM_MASK_FLOAT;
		target_adress2=reinterpret_cast<hk_real*>(adress);
    }

#if 0
    for(int i=size;i>0;i-=hk_VecFPU_SIZE_FLOAT) {
	    __m128d a_d=_mm_load_pd(target_adress1);
	    __m128d b_d=_mm_load_pd(target_adress2);
	    _mm_store_pd(target_adress1,b_d);    
	    _mm_store_pd(target_adress2,a_d);    
	    
	    target_adress1+=hk_VecFPU_SIZE_FLOAT;
	    target_adress2+=hk_VecFPU_SIZE_FLOAT;
	}
#endif

    for(int i=size;i>0;i-=hk_VecFPU_SIZE_FLOAT) {
#if hk_VecFPU_SIZE_FLOAT == 2
	    hk_real h;
	    h=target_adress1[0];  target_adress1[0]=target_adress2[0];  target_adress2[0]=h;
        h=target_adress1[1];  target_adress1[1]=target_adress2[1];  target_adress2[1]=h;
#elif hk_VecFPU_SIZE_FLOAT == 4
	    std::swap(target_adress1[0], target_adress2[0]);
	    std::swap(target_adress1[1], target_adress2[1]);
	    std::swap(target_adress1[2], target_adress2[2]);
	    std::swap(target_adress1[3], target_adress2[3]);
#else
#error Unknown float size
#endif

	    target_adress1+=hk_VecFPU_SIZE_FLOAT;
	    target_adress2+=hk_VecFPU_SIZE_FLOAT;
	}
}

inline void hk_VecFPU::fpu_copy_rows(hk_real *target_adress,hk_real *source_adress,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		//we have to calculate the block size and shift adresses to lower aligned adresses
		hk_intp adress=reinterpret_cast<hk_intp>(source_adress);
		hk_intp result_adress=adress & hk_VecFPU_MEM_MASK_FLOAT;
		size+=(adress-result_adress)>>hk_VecFPU_MEMSHIFT_FLOAT;
		source_adress=reinterpret_cast<hk_real*>(result_adress);
		adress=reinterpret_cast<hk_intp>(target_adress);
		adress=adress & hk_VecFPU_MEM_MASK_FLOAT;
		target_adress=reinterpret_cast<hk_real*>(adress);
    }

#if 0
    for(int i=size-1;i>=0;i-=hk_VecFPU_SIZE_FLOAT) {
	    __m128d target_d=_mm_load_pd(source_adress);
	    _mm_store_pd(target_adress,target_d);

	    target_adress+=hk_VecFPU_SIZE_FLOAT;
	    source_adress+=hk_VecFPU_SIZE_FLOAT;
	}
#endif

    for(int i=size-1;i>=0;i-=hk_VecFPU_SIZE_FLOAT) {
		for(int j=0;j<hk_VecFPU_SIZE_FLOAT;j++) {
			target_adress[j] = source_adress[j];
		}

		target_adress+=hk_VecFPU_SIZE_FLOAT;
		source_adress+=hk_VecFPU_SIZE_FLOAT;
	}
}

inline void hk_VecFPU::fpu_set_row_to_zero(hk_real *target_adress,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		hk_intp adress=reinterpret_cast<hk_intp>(target_adress);
		hk_intp result_adress=adress & hk_VecFPU_MEM_MASK_FLOAT;
		size+=(adress-result_adress)>>hk_VecFPU_MEMSHIFT_FLOAT;
		target_adress=reinterpret_cast<hk_real*>(result_adress);
    }

#if 0
    __m128d zero128=_mm_set1_pd(0.0f);
    int i;
    for(i=size-1;i>=0;i-=hk_VecFPU_SIZE_FLOAT) {	
	    _mm_store_pd(target_adress,zero128);

	    target_adress+=hk_VecFPU_SIZE_FLOAT;
	} 
#endif

    for(int i=size-1;i>=0;i-=hk_VecFPU_SIZE_FLOAT) {
	    for(int j=0;j<hk_VecFPU_SIZE_FLOAT;j++) {
	        target_adress[j] = 0.0f;
		}

	    target_adress+=hk_VecFPU_SIZE_FLOAT;
	}
}

inline int hk_VecFPU::calc_aligned_row_len(int unaligned_len, [[maybe_unused]] hk_real *dummy_type) { //dummy type is used for overloading
    return (unaligned_len+hk_VecFPU_SIZE_FLOAT-1)&hk_VecFPU_MASK_FLOAT;
}

//----------------------------------------------------------------------------------------------------------------------



inline void hk_VecFPU::fpu_add_multiple_row(hk_double *target_adress,hk_double *source_adress,hk_double factor,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		//we have to calculate the block size and shift adresses to lower aligned adresses
		hk_intp result_adress = reinterpret_cast<hk_intp>(source_adress) & hk_VecFPU_MEM_MASK_DOUBLE;
		target_adress = (hk_double *)( reinterpret_cast<hk_intp>(target_adress) & hk_VecFPU_MEM_MASK_DOUBLE);
		size += (reinterpret_cast<hk_intp>(source_adress)-result_adress)>>hk_VecFPU_MEMSHIFT_DOUBLE;
		source_adress=reinterpret_cast<hk_double*>(result_adress);
    }

	if(0) {
#if defined(IVP_WILLAMETTE)
        __m128d factor128=_mm_set1_pd(factor);

        for(int i=size;i>0;i-=hk_VecFPU_SIZE_DOUBLE) {
	        __m128d source_d=_mm_load_pd(source_adress);
	        __m128d prod_d=_mm_mul_pd(factor128,source_d);
	        __m128d target_d=_mm_load_pd(target_adress);
	        target_d=_mm_add_pd(prod_d,target_d);
	        _mm_store_pd(target_adress,target_d);

	        target_adress+=hk_VecFPU_SIZE_DOUBLE;
	        source_adress+=hk_VecFPU_SIZE_DOUBLE;
		}
#endif
	} else {
        for(int i=size;i>0;i-=hk_VecFPU_SIZE_DOUBLE) {
#if hk_VecFPU_SIZE_DOUBLE == 2
			hk_double a = source_adress[0] * factor;
			hk_double b = source_adress[1] * factor;
			a += target_adress[0];
			b += target_adress[1];
			target_adress[0] = a;
			target_adress[1] = b;
#elif hk_VecFPU_SIZE_DOUBLE == 4
			hk_double a = source_adress[0] * factor;
			hk_double b = source_adress[1] * factor;
			hk_double c = source_adress[2] * factor;
			hk_double d = source_adress[3] * factor;
			a += target_adress[0];
			b += target_adress[1];
			c += target_adress[2];
			d += target_adress[3];
			target_adress[0] = a;
			target_adress[1] = b;
			target_adress[2] = c;
			target_adress[3] = d;
#else 
#error Unknown double size
#endif

			target_adress+=hk_VecFPU_SIZE_DOUBLE;
			source_adress+=hk_VecFPU_SIZE_DOUBLE;
		}
	}
}

inline hk_double hk_VecFPU::fpu_large_dot_product(hk_double *base_a, hk_double *base_b, int size, hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
	    //we have to calculate the block size and shift adresses to lower aligned adresses
	    hk_intp result_adress = reinterpret_cast<hk_intp>(base_a) & hk_VecFPU_MEM_MASK_DOUBLE;
	    base_b = (hk_double *)( reinterpret_cast<hk_intp>(base_b) & hk_VecFPU_MEM_MASK_DOUBLE);
	    size += (reinterpret_cast<hk_intp>(base_a)-result_adress)>>hk_VecFPU_MEMSHIFT_DOUBLE;  // because start changed
	    base_a=reinterpret_cast<hk_double*>(result_adress);
    }

#if defined(IVP_WILLAMETTE)
    if(0) {
		__m128d sum128 =_mm_set1_pd(0.0);

		int i;
		for(i=size;i>=hk_VecFPU_SIZE_DOUBLE; i-= hk_VecFPU_SIZE_DOUBLE) {
			__m128d mult1=_mm_load_pd(base_a);
			__m128d mult2=_mm_load_pd(base_b);
			__m128d prod =_mm_mul_pd(mult1,mult2);
			sum128 =_mm_add_pd(prod,sum128);
			base_a += hk_VecFPU_SIZE_DOUBLE;
			base_b += hk_VecFPU_SIZE_DOUBLE;
		}

		__m128d dummy,low,high;
		low=sum128;
		dummy=sum128;
		//_mm_shuffle_pd(sum128,sum128,1); //swap high and low
		sum128=_mm_unpackhi_pd(sum128,dummy);
		high=sum128;
		__m128d sum64=_mm_add_sd(low,high);
		__m128d res=sum64;
		//_mm_shuffle_pd(sum64,sum64,1); 
		hk_double result_sum; 
		_mm_store_sd(&result_sum,res);
	
		for(;i>=0;i--) {
			result_sum += base_a[i] * base_b[i];
		}

		return result_sum;
    } else {
		hk_double sum=0.0;
		for(int i=size-1;i>=0;i--) {
			sum += base_a[i] * base_b[i];
		}
		return sum;
    } 
#else
    hk_double sum=0.0;
    for(int i=0;i<size;i++) {
	sum += base_a[i] * base_b[i];
    }
    return sum;
#endif
}

inline void hk_VecFPU::fpu_multiply_row(hk_double *target_adress,hk_double factor,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		//we have to calculate the block size and shift adresses to lower aligned adresses
		hk_intp adress=reinterpret_cast<hk_intp>(target_adress);
		hk_intp result_adress=adress & hk_VecFPU_MEM_MASK_DOUBLE;
		size+=(adress-result_adress)>>hk_VecFPU_MEMSHIFT_DOUBLE;
		target_adress=reinterpret_cast<hk_double*>(result_adress);
    }

	if(0) {
#ifdef IVP_WILLAMETTE
        __m128d factor128=_mm_set1_pd(factor);
        for(int i=size;i>0;i-=hk_VecFPU_SIZE_DOUBLE) {
			__m128d target_d=_mm_load_pd(target_adress);
			target_d=_mm_mul_pd(factor128,target_d);
			_mm_store_pd(target_adress,target_d);
	
			target_adress+=hk_VecFPU_SIZE_DOUBLE;
		}    
#endif
	} else {
        for(int i=size;i>0;i-=hk_VecFPU_SIZE_DOUBLE) {
#if hk_VecFPU_SIZE_DOUBLE == 2
			hk_double a = target_adress[0] * factor;
			hk_double b = target_adress[1] * factor;
			target_adress[0] = a;
			target_adress[1] = b;
#elif hk_VecFPU_SIZE_DOUBLE == 4
			hk_double a = target_adress[0] * factor;
			hk_double b = target_adress[1] * factor;
			hk_double c = target_adress[2] * factor;
			hk_double d = target_adress[3] * factor;
			target_adress[0] = a;
			target_adress[1] = b;
			target_adress[2] = c;
			target_adress[3] = d;
#else
#error Double size is unknown
#endif
			target_adress+=hk_VecFPU_SIZE_DOUBLE;
		}
	}
}

// #+# sparc says rui, optimize for non vector units ( hk_VecFPU_SIZE_DOUBLE = 4 )
inline void hk_VecFPU::fpu_exchange_rows(hk_double *target_adress1,hk_double *target_adress2,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		//we have to calculate the block size and shift adresses to lower aligned adresses
		hk_intp adress=reinterpret_cast<hk_intp>(target_adress1);
		hk_intp result_adress=adress & hk_VecFPU_MEM_MASK_DOUBLE;
		size+=(adress-result_adress)>>hk_VecFPU_MEMSHIFT_DOUBLE;
		target_adress1=reinterpret_cast<hk_double*>(result_adress);
		adress=reinterpret_cast<hk_intp>(target_adress2);
		adress=adress & hk_VecFPU_MEM_MASK_DOUBLE;
		target_adress2=reinterpret_cast<hk_double *>(adress);
    }

    if(0) {
#ifdef IVP_WILLAMETTE
        for(int i=size;i>0;i-=hk_VecFPU_SIZE_DOUBLE) {
			__m128d a_d=_mm_load_pd(target_adress1);
			__m128d b_d=_mm_load_pd(target_adress2);
			_mm_store_pd(target_adress1,b_d);    
			_mm_store_pd(target_adress2,a_d);    
	    
			target_adress1+=hk_VecFPU_SIZE_DOUBLE;
			target_adress2+=hk_VecFPU_SIZE_DOUBLE;
		}
#endif
	} else {
        for(int i=size;i>0;i-=hk_VecFPU_SIZE_DOUBLE) {
#if hk_VecFPU_SIZE_DOUBLE == 2
	        std::swap(target_adress1[0], target_adress2[0]);
	        std::swap(target_adress1[1], target_adress2[1]);
#elif hk_VecFPU_SIZE_DOUBLE == 4
	        hk_double h;
	        h=target_adress1[0];  target_adress1[0]=target_adress2[0];  target_adress2[0]=h;
	        h=target_adress1[1];  target_adress1[1]=target_adress2[1];  target_adress2[1]=h;
	        h=target_adress1[2];  target_adress1[2]=target_adress2[2];  target_adress2[2]=h;
	        h=target_adress1[3];  target_adress1[3]=target_adress2[3];  target_adress2[3]=h;
#else
#error Unknown double size
#endif

			target_adress1+=hk_VecFPU_SIZE_DOUBLE;
			target_adress2+=hk_VecFPU_SIZE_DOUBLE;
		}
	}
}

inline void hk_VecFPU::fpu_copy_rows(hk_double *target_adress,hk_double *source_adress,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		//we have to calculate the block size and shift adresses to lower aligned adresses
		hk_intp adress=reinterpret_cast<hk_intp>(source_adress);
		hk_intp result_adress=adress & hk_VecFPU_MEM_MASK_DOUBLE;
		size+=(adress-result_adress)>>hk_VecFPU_MEMSHIFT_DOUBLE;
		source_adress=reinterpret_cast<hk_double*>(result_adress);
		adress=reinterpret_cast<hk_intp>(target_adress);
		adress=adress & hk_VecFPU_MEM_MASK_DOUBLE;
		target_adress=reinterpret_cast<hk_double *>(adress);
    }

#ifdef HK_WILLAMETTE
    for(int i=size-1;i>=0;i-=hk_VecFPU_SIZE_DOUBLE) {
	    __m128d target_d=_mm_load_pd(source_adress);
	    _mm_store_pd(target_adress,target_d);

	    target_adress+=hk_VecFPU_SIZE_DOUBLE;
	    source_adress+=hk_VecFPU_SIZE_DOUBLE;
	}
#else

    for(int i=size-1;i>=0;i-=hk_VecFPU_SIZE_DOUBLE) {
	    for(int j=0;j<hk_VecFPU_SIZE_DOUBLE;j++) {
	        target_adress[j] = source_adress[j];
		}

	    target_adress+=hk_VecFPU_SIZE_DOUBLE;
	    source_adress+=hk_VecFPU_SIZE_DOUBLE;
	}
#endif
}

inline void hk_VecFPU::fpu_set_row_to_zero(hk_double *target_adress,int size,hk_bool adress_aligned) {
    if(adress_aligned==HK_FALSE) {
		hk_intp adress=reinterpret_cast<hk_intp>(target_adress);
		hk_intp result_adress=adress & hk_VecFPU_MEM_MASK_DOUBLE;
		size+=(adress-result_adress)>>hk_VecFPU_MEMSHIFT_DOUBLE;
		target_adress=reinterpret_cast<hk_double*>(result_adress);
    }

	if(0) {
#ifdef IVP_WILLAMETTE
        __m128d zero128=_mm_set1_pd(0.0f);
        for(int i=size-1;i>=0;i-=hk_VecFPU_SIZE_DOUBLE) {	
	        _mm_store_pd(target_adress,zero128);

	        target_adress+=hk_VecFPU_SIZE_DOUBLE;
		} 
#endif
	} else {
        for(int i=size-1;i>=0;i-=hk_VecFPU_SIZE_DOUBLE) {
	        for(int j=0;j<hk_VecFPU_SIZE_DOUBLE;j++) {
	            target_adress[j] = 0.0;
			}
	        target_adress+=hk_VecFPU_SIZE_DOUBLE;
		}     
	}
}

inline int hk_VecFPU::calc_aligned_row_len(int unaligned_len, [[maybe_unused]] hk_double *dummy_type) { //dummy type is used for overloading
    return (unaligned_len+hk_VecFPU_SIZE_DOUBLE-1)&hk_VecFPU_MASK_DOUBLE;
}

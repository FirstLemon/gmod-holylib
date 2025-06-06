#ifndef HK_PHYSICS_HINGE_CONTRAINT_H
#define HK_PHYSICS_HINGE_CONTRAINT_H

// IVP_EXPORT_PUBLIC

class hk_Hinge_BP;

class hk_Hinge_Constraint : public hk_Constraint
{
	public:

		hk_Hinge_Constraint( hk_Environment*, const hk_Hinge_BP* ,
							hk_Rigid_Body *a ,hk_Rigid_Body *b );	
		hk_Hinge_Constraint( hk_Local_Constraint_System *, const hk_Hinge_BP *, hk_Rigid_Body *a ,hk_Rigid_Body *b );	

		int get_vmq_storage_size() override;

		int	setup_and_step_constraint(
				hk_PSI_Info& pi,
				void *mem,
				hk_real tau_factor,
				hk_real strength_factor ) override;

		void step_constraint(
				hk_PSI_Info& pi,
				void *mem,
				hk_real tau_factor,
				hk_real strength_factor ) override;

		void apply_effector_PSI( hk_PSI_Info&, hk_Array<hk_Entity*>* );

		void init_constraint(const void*) override;
		void set_limits( hk_real min, hk_real max );
		void set_friction( hk_real friction );
		void set_motor( hk_real desired_vel, hk_real max_force );
		void write_to_blueprint( hk_Hinge_BP * );

		const char* get_constraint_type() override
		{
			return "hinge";
		}

		int get_constraint_dof() override
		{
			return 1;
		}

	protected:

		void init_hinge_constraint(const hk_Hinge_BP*);

		hk_Ray m_axis_os[2];
		hk_Vector3 m_axis_perp_os[2];


		hk_real m_tau;			//XXX rename me to m_tau - this is drift stabilization paramater
		hk_real m_strength;		//XXX rename me to m_strength or similar - 1.0 is pure velocity constraint

		hk_Constraint_Limit m_limit; // XXX same as max_force for motor
};

#endif /* HK_PHYSICS_HINGE_CONTRAINT_H */


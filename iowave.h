/*--------------------------------------------------------------------
REEF3D
Copyright 2008-2019 Hans Bihs

This file is part of REEF3D.

REEF3D is fra->eps software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Fra->eps Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. Sa->eps the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, sa->eps <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------
--------------------------------------------------------------------*/

#include"ioflow.h"
#include"wave_interface.h"
#include"field1.h"
#include"field2.h"
#include"field4.h"
#include"slice4.h"
#include"flowfile_in.h"

class vec;
class vrans;
class fdm_fnpf;

using namespace std;

#ifndef IOWAVE_RELAX_H_
#define IOWAVE_RELAX_H_

class iowave : public ioflow, public wave_interface, public increment, public flowfile_in
{

public:
	iowave(lexer*, ghostcell*);
	virtual ~iowave();
	virtual void gcio_update(lexer*,fdm*,ghostcell*);
	virtual void inflow_walldist(lexer*,fdm*,ghostcell*,convection*,reini*,ioflow*);
	virtual void fsfinflow(lexer*,fdm*,ghostcell*);
	virtual void discharge(lexer*,fdm*,ghostcell*);
	virtual void inflow(lexer*,fdm*,ghostcell*,field&,field&,field&);
    virtual void inflow_plain(lexer*,fdm*,ghostcell*,field&,field&,field&);
	virtual void rkinflow(lexer*,fdm*,ghostcell*,field&,field&,field&);
	virtual void fsfrkin(lexer*,fdm*,ghostcell*,field&);
	virtual void fsfrkout(lexer*,fdm*,ghostcell*,field&);
	virtual void fsfrkinV(lexer*,fdm*,ghostcell*,vec&);
	virtual void fsfrkoutV(lexer*,fdm*,ghostcell*,vec&);
	virtual void fsfrkinVa(lexer*,fdm*,ghostcell*,vec&);
	virtual void fsfrkoutVa(lexer*,fdm*,ghostcell*,vec&);
	virtual void iogcb_update(lexer*,fdm*,ghostcell*);
	virtual void isource(lexer*,fdm*,ghostcell*);
    virtual void jsource(lexer*,fdm*,ghostcell*);
    virtual void ksource(lexer*,fdm*,ghostcell*);
    virtual void pressure_io(lexer*,fdm*,ghostcell*);
    virtual void turbulence_io(lexer*,fdm*,ghostcell*);
    virtual void veltimesave(lexer*,fdm*,ghostcell*);
    virtual void Qin(lexer*,fdm*,ghostcell*);
	virtual void Qout(lexer*,fdm*,ghostcell*);
    
    virtual void flowfile(lexer*,fdm*,ghostcell*,turbulence*);
    
    void hydrograph_in_read(lexer*,fdm*,ghostcell*);
	void hydrograph_out_read(lexer*,fdm*,ghostcell*);
	double hydrograph_ipol(lexer*,fdm*,ghostcell*,double**,int);
	
    
    void wavegen_precalc_space(lexer*,ghostcell*);
    void wavegen_precalc_time(lexer*,ghostcell*);
    void wavegen_precalc_decomp_relax(lexer*,ghostcell*);
    void wavegen_precalc_decomp_dirichlet(lexer*,ghostcell*);
    
    virtual void u_relax(lexer*,fdm*,ghostcell*,field&);
    virtual void v_relax(lexer*,fdm*,ghostcell*,field&);
    virtual void w_relax(lexer*,fdm*,ghostcell*,field&);
    virtual void p_relax(lexer*,fdm*,ghostcell*,field&);
	virtual void phi_relax(lexer*,ghostcell*,field&);
    virtual void vof_relax(lexer*,ghostcell*,field&);
    virtual void fi_relax(lexer*,ghostcell*,field&,field&);
    virtual void fivec_relax(lexer*, ghostcell*, double*);
    virtual void fifsf_relax(lexer*, ghostcell*, slice&);
    virtual void visc_relax(lexer*, ghostcell*, slice&);
    virtual void eta_relax(lexer*,ghostcell*,slice&);
    virtual void um_relax(lexer*,ghostcell*,slice&,slice&,slice&);
    virtual void vm_relax(lexer*,ghostcell*,slice&,slice&,slice&);
	virtual void wm_relax(lexer*,ghostcell*,slice&,slice&,slice&);
    virtual void ws_relax(lexer*,ghostcell*,slice&,slice&,slice&);
	virtual void pm_relax(lexer*,ghostcell*,slice&);
    
    
    // 2D
    virtual void wavegen_2D_precalc(lexer*,fdm2D*,ghostcell*);
    virtual void wavegen_2D_precalc_ini(lexer*,ghostcell*);
    
    
    virtual void discharge2D(lexer*,fdm2D*,ghostcell*);
    virtual void Qin2D(lexer*,fdm2D*,ghostcell*);
	virtual void Qout2D(lexer*,fdm2D*,ghostcell*);
    virtual void inflow2D(lexer*,fdm2D*,ghostcell*,slice&,slice&,slice&,slice&);
	virtual void rkinflow2D(lexer*,fdm2D*,ghostcell*,slice&,slice&,slice&,slice&);
	virtual void isource2D(lexer*,fdm2D*,ghostcell*);
    virtual void jsource2D(lexer*,fdm2D*,ghostcell*);
	virtual void full_initialize2D(lexer*,fdm2D*,ghostcell*);
    
    void wavegen2D(lexer*,fdm2D*,ghostcell*,slice&,slice&,slice&,slice&);
    void active_beach2D(lexer*,fdm2D*,ghostcell*,slice&,slice&,slice&,slice&);
    void inflow2D_plain(lexer*,fdm2D*,ghostcell*,slice&,slice&,slice&);
    
	
    virtual double wave_fsf(lexer*,ghostcell*,double);
	virtual int iozonecheck(lexer*,fdm*);
	virtual void full_initialize(lexer*,fdm*,ghostcell*);
    virtual void full_initialize_fnpf(lexer*,fdm_fnpf*,ghostcell*);
	virtual void active_beach(lexer*,fdm*,ghostcell*,field&,field&,field&);
	virtual void active_wavegen(lexer*,fdm*,ghostcell*,field&,field&,field&);
	virtual void active_wavegen2(lexer*,fdm*,ghostcell*,field&,field&,field&);
	virtual void dirichlet_wavegen(lexer*,fdm*,ghostcell*,field&,field&,field&);
    
    virtual void ini(lexer*,fdm*,ghostcell*);
    virtual void ini_fnpf(lexer*,fdm_fnpf*,ghostcell*);
    virtual void ini2D(lexer*,fdm2D*,ghostcell*);
    
    virtual void vrans_sed_update(lexer*,fdm*,ghostcell*);

    void velini(lexer*,fdm*,ghostcell*);
    void pressure_outlet(lexer*,fdm*,ghostcell*);
    void pressure_inlet(lexer*,fdm*,ghostcell*);
    void pressure_wall(lexer*,fdm*,ghostcell*);
    void pressure_bed(lexer*,fdm*,ghostcell*);
    double local_fsf(lexer*,fdm*,ghostcell*);
	
	void awa_ini(lexer*,fdm*,ghostcell*);
	void awa_update(lexer*,fdm*,ghostcell*);
	void gen_ini(lexer*,fdm*,ghostcell*);
	
	
    // precalc
	virtual void wavegen_precalc(lexer*,ghostcell*);
    virtual void wavegen_precalc_ini(lexer*,ghostcell*);
    
    
    void wavegen_precalc_relax(lexer*,ghostcell*);
    void wavegen_precalc_relax_ini(lexer*,ghostcell*);
    void wavegen_precalc_dirichlet(lexer*,ghostcell*);
    void wavegen_precalc_dirichlet_ini(lexer*,ghostcell*);
    
    
    // FNPF
    virtual void inflow_fnpf(lexer*,ghostcell*,double*,double*,slice&,slice&);
    void fnpf_precalc_relax(lexer*,ghostcell*);
    void fnpf_precalc_relax_ini(lexer*,ghostcell*);
    void fnpf_precalc_dirichlet(lexer*,ghostcell*);
    void fnpf_precalc_dirichlet_ini(lexer*,ghostcell*);
    void dirichlet_wavegen_fnpf(lexer*,ghostcell*,double*,double*,slice&,slice&);
    void active_beach_fnpf(lexer*, ghostcell*, double*, double*, slice&, slice&);
    
    void wavegen_precalc_decomp_space_fnpf(lexer*,ghostcell*);
    void wavegen_precalc_decomp_time_fnpf(lexer*,ghostcell*);
    void wavegen_precalc_decomp_relax_fnpf(lexer*,ghostcell*);
    void wavegen_precalc_decomp_dirichlet_fnpf(lexer*,ghostcell*);
    
    // NHFLOW
    //virtual void nhflow_inflow(lexer*,ghostcell*,double*,double*,slice&,slice&);
    void nhflow_precalc_relax(lexer*,ghostcell*);
    void nhflow_precalc_relax_ini(lexer*,ghostcell*);
    void nhflow_precalc_dirichlet(lexer*,ghostcell*);
    void nhflow_precalc_dirichlet_ini(lexer*,ghostcell*);
    void nhflow_dirichlet_wavegen(lexer*,ghostcell*,double*,double*,slice&,slice&);
    void nhflow_active_beach(lexer*, ghostcell*, double*, double*, slice&, slice&);
	

private:
    slice4 eta;
	
	double rb1(lexer*,double);
    double rb3(lexer*,double);

    double ramp(lexer*);
	
	double xgen(lexer*);
    double xgen1(lexer*);
    double xgen2(lexer*);
    double ygen(lexer*);
    double ygen1(lexer*);
    double ygen2(lexer*);

	double distgen(lexer*);
	double distbeach(lexer*);
    
    double distbeach_ini(lexer*);
    double distgen_ini(lexer*);
    int intriangle(lexer*,double,double,double,double,double,double,double,double);

    int n,count;
    int wtype;
    double inflow_bed,uvel,vvel,wvel;
    double area,Ai,Ao,Ui,fac;
    double dist1,dist2,dist3,dist3_fac;
    double x,y,z;
    double x1,y1,x2,y2,z3;
	double xg,yg,dg,db;
    int gcval_press;
    const double epsi,psi;
	double alpha,*beta,gamma;
    double H,G,phival;
	double kinval,epsval;
	double tan_alpha,*tan_beta;
	double wh;
	
	int gcawa1_count,gcawa2_count,gcawa3_count,gcawa4_count;
	int **gcawa1,**gcawa2,**gcawa3,**gcawa4;
	
	int gcgen1_count,gcgen2_count,gcgen3_count,gcgen4_count;
	int **gcgen1,**gcgen2,**gcgen3,**gcgen4;
    
    // relax points
    double **G1,**G2,**G3,**G4,**Gs,**Ge;
    double **B1,**B2,**B3,**B4,**Bs,**Be;
    
    // relax pre-calc
    int wave_comp;
    int upt_count,vpt_count,wpt_count,ppt_count,ept_count;
    double *uval,*vval,*wval,*etaval,*lsval,*Fival,*Fioutval,*Fifsfval,*Fifsfoutval,*Uinval,*Uoutval;
    double *rb1val,*rb3val;
    
    double **uval_S_sin,**vval_S_sin,**wval_S_sin,**etaval_S_sin,**Fival_S_sin,**Fifsfval_S_sin;
    double **uval_S_cos,**vval_S_cos,**wval_S_cos,**etaval_S_cos,**Fival_S_cos,**Fifsfval_S_cos;
    
    double *uval_T_sin,*vval_T_sin,*wval_T_sin,*etaval_T_sin,*Fival_T_sin,*Fifsfval_T_sin;
    double *uval_T_cos,*vval_T_cos,*wval_T_cos,*etaval_T_cos,*Fival_T_cos,*Fifsfval_T_cos;
    
    double zloc1,zloc2,zloc3,zloc4;

    
	double **wsfmax;
    
    double Apor,Bpor,porval,partval;
	
	int u_switch,v_switch,w_switch,p_switch,h_switch,f_switch;
    
    double expinverse;
	
    double **hydro_in,**hydro_out;
    int hydro_in_count,hydro_out_count;
    
	vrans *pvrans;
};

#endif



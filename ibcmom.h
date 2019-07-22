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

#include"surftens.h"
#include"roughness.h"
class lexer;
class fdm;
class ghostcell;
class field;
class turbulence;

#ifndef IBCMOM_H_
#define IBCMOM_H_

using namespace std;

class ibcmom : public surftens, public roughness
{
public:
	ibcmom(lexer*);
	virtual ~ibcmom();
	virtual void ibcmom_start(fdm*,lexer*,ghostcell*,turbulence*,field&, int);
	void wall_law_u(fdm*,lexer*,turbulence*,field&,int,int,int,int,int,int,double);
	void wall_law_v(fdm*,lexer*,turbulence*,field&,int,int,int,int,int,int,double);
	void wall_law_w(fdm*,lexer*,turbulence*,field&,int,int,int,int,int,int,double);

private:
	const double kappa;
	double uplus,ks_plus,dist,ks,ustar;
	int ii,jj,kk;
	int q,qn;
	double dx,value;
	int gcval_phi, wallfunc_type;
};
#endif
/*--------------------------------------------------------------------
REEF3D
Copyright 2008-2019 Hans Bihs

This file is part of REEF3D.

REEF3D is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------
--------------------------------------------------------------------*/

#include"fnpf_sg_coastline.h"
#include"lexer.h"
#include"ghostcell.h"
#include"slice.h"
#include"sliceint.h"

fnpf_sg_coastline::fnpf_sg_coastline(lexer* p) :  ddweno_f_nug(p), frk1(p),frk2(p),L(p),dt(p),wet_n(p)
{
    time_preproc(p); 
}

fnpf_sg_coastline::~fnpf_sg_coastline()
{
}

void fnpf_sg_coastline::start(lexer *p, ghostcell *pgc, slice &coastline, sliceint &wet, sliceint &wet_n)
{
    // if p->count==0, ini
    if(p->count==0)
    {
        SLICELOOP4
        {
            if(wet(i,j)==0)
            coastline(i,j)=-1.0;
            
            if(wet(i,j)==1)
            coastline(i,j)=1.0;
            
            
        }
        reini(p,pgc,coastline);

    }
    
    
    // if p->count>0, check for wetdry changes
    change=0;
    if(p->count>0)
    {
        SLICELOOP4
        {
            if(wet(i,j)==0 && wet_n(i,j)==1)
            {
            coastline(i,j) = -0.25*(p->DXN[IP]+p->DYN[JP]);
            change+=1;
            }
            
            if(wet(i,j)==1 && wet_n(i,j)==0)
            {
            coastline(i,j) =  0.25*(p->DXN[IP]+p->DYN[JP]);
            change+=1;
            }
        }
        
        change=pgc->globalisum(change);
        
        //if(p->mpirank==0)
        //cout<<p->mpirank<<" wetdry change: "<<change<<endl;
        
        if(change>0)
        reini(p,pgc,coastline);
    }
    
    

    
}
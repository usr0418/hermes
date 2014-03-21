// This file is part of Hermes2D.
//
// Hermes2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Hermes2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hermes2D.  If not, see <http://www.gnu.org/licenses/>.

/// \file This file contains definition of methods of classes for form evaluation (hence the name) Geom, and Func.

#include "forms.h"
#include "api2d.h"
#include <complex>

namespace Hermes
{
  namespace Hermes2D
  {
    Func<double>::Func() : np(-1), nc(-1)
    {
    }

    Func<double>::Func(int np, int nc) : np(np), nc(nc)
    {
    }

    Func<std::complex<double> >::Func() : np(-1), nc(-1)
    {
    }

    Func<std::complex<double> >::Func(int np, int nc) : np(np), nc(nc)
    {
    }

    Func<Hermes::Ord>::Func(const int order) : order(order)
    {
      Hermes::Ord d(order);
      Hermes::Ord d1(order > 1 ? order - 1 : order);

      this->val = this->val0 = this->val1 = d;
      this->dx = this->dy = this->curl = this->div = d1;

#ifdef H2D_USE_SECOND_DERIVATIVES
      Hermes::Ord d2(std::min(0, order - 2));
      this->laplace = d2;
#endif
    }

    void Func<double>::subtract(Func<double>* func)
    {
      if (this->np != func->np)
        throw Hermes::Exceptions::Exception("Unable to subtract a function due to a different number of integration points (this: %d, other: %d)", np, func->np);
      if (nc != func->nc)
        throw Hermes::Exceptions::Exception("Unable to subtract a function due to a different number of components (this: %d, other: %d)", nc, func->nc);

      subtract(this->val, func->val);
      subtract(this->dx, func->dx);
      subtract(this->dy, func->dy);

#ifdef H2D_USE_SECOND_DERIVATIVES
      subtract(this->laplace, func->laplace);
#endif

      if (nc > 1)
      {
        subtract(this->val0, func->val0);
        subtract(this->val1, func->val1);
        subtract(this->curl, func->curl);
        subtract(this->div, func->div);
      }
    };

    void Func<std::complex<double> >::subtract(Func<std::complex<double> >* func)
    {
      if (this->np != func->np)
        throw Hermes::Exceptions::Exception("Unable to subtract a function due to a different number of integration points (this: %d, other: %d)", np, func->np);
      if (nc != func->nc)
        throw Hermes::Exceptions::Exception("Unable to subtract a function due to a different number of components (this: %d, other: %d)", nc, func->nc);

      subtract(this->val, func->val);
      subtract(this->dx, func->dx);
      subtract(this->dy, func->dy);

#ifdef H2D_USE_SECOND_DERIVATIVES
      subtract(this->laplace, func->laplace);
#endif

      if (nc > 1)
      {
        subtract(this->val0, func->val0);
        subtract(this->val1, func->val1);
        subtract(this->curl, func->curl);
        subtract(this->div, func->div);
      }
    };

    void Func<double>::subtract(double* attribute, double* other_attribute)
    {
      if (attribute != nullptr && other_attribute != nullptr)
      {
        for (int i = 0; i < this->np; i++)
          attribute[i] -= other_attribute[i];
      }
    }

    void Func<std::complex<double> >::subtract(std::complex<double> * attribute, std::complex<double> * other_attribute)
    {
      if (attribute != nullptr && other_attribute != nullptr)
      {
        for (int i = 0; i < this->np; i++)
          attribute[i] -= other_attribute[i];
      }
    }

    void Func<double>::add(Func<double>* func)
    {
      if (this->np != func->np)
        throw Hermes::Exceptions::Exception("Unable to add a function due to a different number of integration points (this: %d, other: %d)", np, func->np);
      if (nc != func->nc)
        throw Hermes::Exceptions::Exception("Unable to add a function due to a different number of components (this: %d, other: %d)", nc, func->nc);

      add(this->val, func->val);
      add(this->dx, func->dx);
      add(this->dy, func->dy);

#ifdef H2D_USE_SECOND_DERIVATIVES
      add(this->laplace, func->laplace);
#endif

      if (nc > 1)
      {
        add(this->val0, func->val0);
        add(this->val1, func->val1);
        add(this->curl, func->curl);
        add(this->div, func->div);
      }
    };

#ifdef H2D_USE_SECOND_DERIVATIVES
    template<>
#endif
    void Func<std::complex<double> >::add(Func<std::complex<double> >* func)
    {
      if (this->np != func->np)
        throw Hermes::Exceptions::Exception("Unable to add a function due to a different number of integration points (this: %d, other: %d)", np, func->np);
      if (nc != func->nc)
        throw Hermes::Exceptions::Exception("Unable to add a function due to a different number of components (this: %d, other: %d)", nc, func->nc);

      add(this->val, func->val);
      add(this->dx, func->dx);
      add(this->dy, func->dy);

#ifdef H2D_USE_SECOND_DERIVATIVES
      add(this->laplace, func->laplace);
#endif

      if (nc > 1)
      {
        add(this->val0, func->val0);
        add(this->val1, func->val1);
        add(this->curl, func->curl);
        add(this->div, func->div);
      }
    };

    void Func<double>::add(double* attribute, double* other_attribute)
    {
      if (attribute != nullptr && other_attribute != nullptr)
      {
        for (int i = 0; i < this->np; i++)
          attribute[i] += other_attribute[i];
      }
    }

    void Func<std::complex<double> >::add(std::complex<double> * attribute, std::complex<double> * other_attribute)
    {
      if (attribute != nullptr && other_attribute != nullptr)
      {
        for (int i = 0; i < this->np; i++)
          attribute[i] += other_attribute[i];
      }
    }

    template<typename T>
    DiscontinuousFunc<T>::DiscontinuousFunc(Func<T>* fn, bool support_on_neighbor, bool reverse) :
      Func<T>(fn->np, fn->nc), fn_central(nullptr), fn_neighbor(nullptr), reverse_neighbor_side(reverse)
    {
        if (fn == nullptr)
          throw Hermes::Exceptions::Exception("Invalid arguments to DiscontinuousFunc constructor.");
        if (support_on_neighbor)
        {
          fn_neighbor = fn;
          if (reverse_neighbor_side)
          {
            this->val_neighbor = malloc_with_check<DiscontinuousFunc<T>, T>(this->np, this);
            this->dx_neighbor = malloc_with_check<DiscontinuousFunc<T>, T>(this->np, this);
            this->dy_neighbor = malloc_with_check<DiscontinuousFunc<T>, T>(this->np, this);
            for (int i = 0; i < this->np; i++)
            {
              this->val_neighbor[i] = fn->val[this->np - i - 1];
              this->dx_neighbor[i] = fn->dx[this->np - i - 1];
              this->dy_neighbor[i] = fn->dy[this->np - i - 1];
            }
          }
          else
          {
            this->val_neighbor = fn->val;
            this->dx_neighbor = fn->dx;
            this->dy_neighbor = fn->dy;
          }

          this->val = this->dx = this->dy = nullptr;
        }
        else
        {
          this->fn_central = fn;
          this->val = fn->val;
          this->dx = fn->dx;
          this->dy = fn->dy;
          this->val_neighbor = this->dx_neighbor = this->dy_neighbor = nullptr;
        }
      }

    template<typename T>
    DiscontinuousFunc<T>::DiscontinuousFunc(Func<T>* fn_c, Func<T>* fn_n, bool reverse) :
      Func<T>(fn_c->np, fn_c->nc), fn_central(fn_c), fn_neighbor(fn_n), reverse_neighbor_side(reverse)
    {
        if (reverse_neighbor_side)
        {
          this->val_neighbor = malloc_with_check<DiscontinuousFunc<T>, T>(this->np, this);
          this->dx_neighbor = malloc_with_check<DiscontinuousFunc<T>, T>(this->np, this);
          this->dy_neighbor = malloc_with_check<DiscontinuousFunc<T>, T>(this->np, this);
          for (int i = 0; i < this->np; i++)
          {
            this->val_neighbor[i] = fn_neighbor->val[this->np - i - 1];
            this->dx_neighbor[i] = fn_neighbor->dx[this->np - i - 1];
            this->dy_neighbor[i] = fn_neighbor->dy[this->np - i - 1];
          }
        }
        else
        {
          this->val_neighbor = fn_neighbor->val;
          this->dx_neighbor = fn_neighbor->dx;
          this->dy_neighbor = fn_neighbor->dy;
        }
        this->val = fn_central->val;
        this->dx = fn_central->dx;
        this->dy = fn_central->dy;
      }

    DiscontinuousFunc<Ord>::DiscontinuousFunc(Func<Ord>* fn, bool support_on_neighbor, bool reverse) : Func<Ord>(fn->order),
      fn_central(nullptr), fn_neighbor(nullptr), reverse_neighbor_side(reverse)
    {
      if (fn == nullptr)
        throw Hermes::Exceptions::Exception("Invalid arguments to DiscontinuousFunc constructor.");
      if (support_on_neighbor)
      {
        this->fn_neighbor = fn;
        this->val_neighbor = fn->val;
        this->dx_neighbor = fn->dx;
        this->dy_neighbor = fn->dy;
      }
      else
      {
        this->fn_central = fn;
        this->val = fn->val;
        this->dx = fn->dx;
        this->dy = fn->dy;
      }
    }

    DiscontinuousFunc<Ord>::DiscontinuousFunc(Func<Ord>* fn_c, Func<Ord>* fn_n, bool reverse) : Func<Ord>(std::max(fn_c->order, fn_n->order)),
      fn_central(fn_c), fn_neighbor(fn_n), reverse_neighbor_side(reverse)
    {
      this->val_neighbor = fn_neighbor->val;
      this->dx_neighbor = fn_neighbor->dx;
      this->dy_neighbor = fn_neighbor->dy;

      this->val = fn_central->val;
      this->dx = fn_central->dx;
      this->dy = fn_central->dy;
    }

    template<typename T>
    void DiscontinuousFunc<T>::subtract(const DiscontinuousFunc<T>& func)
    {
      if (fn_central != nullptr && func.fn_central != nullptr)
        fn_central->subtract(func.fn_central);
      if (fn_neighbor != nullptr && func.fn_neighbor != nullptr)
      {
        fn_neighbor->subtract(func.fn_neighbor);
        if (reverse_neighbor_side)
        {
          Func<T>::subtract(this->val_neighbor, func.val_neighbor);
          Func<T>::subtract(this->dx_neighbor, func.dx_neighbor);
          Func<T>::subtract(this->dy_neighbor, func.dy_neighbor);
        }
      }
    }

    template<typename T>
    DiscontinuousFunc<T>::~DiscontinuousFunc()
    {
      free();
    }

    template<typename T>
    void DiscontinuousFunc<T>::free()
    {
      if (fn_central != nullptr)
      {
        delete fn_central;
        fn_central = nullptr;
      }
      if (fn_neighbor != nullptr)
      {
        if (reverse_neighbor_side)
        {
          free_with_check(this->val_neighbor);
          free_with_check(this->dx_neighbor);
          free_with_check(this->dy_neighbor);
        }
        delete fn_neighbor;
        fn_neighbor = nullptr;
      }
    }

    template<>
    double GeomVol<double>::get_diam_approximation(unsigned char n)
    {
      double x_min = std::numeric_limits<double>::max(),
        x_max = std::numeric_limits<double>::min(),
        y_min = std::numeric_limits<double>::max(),
        y_max = std::numeric_limits<double>::min();

      for (unsigned char i = 0; i < n; i++)
      {
        if (this->x[i] < x_min)
          x_min = this->x[i];
        if (this->x[i] > x_max)
          x_max = this->x[i];

        if (this->y[i] < y_min)
          y_min = this->y[i];
        if (this->y[i] > y_max)
          y_max = this->y[i];
      }

      return std::sqrt((x_max - x_min) * (x_max - x_min) + (y_max - y_min) * (y_max - y_min));
    }

    template<>
    double GeomVol<double>::get_area(unsigned char n, double* wt)
    {
      double area = 0.;
      for (unsigned char i = 0; i < n; i++)
        area += wt[i];
      return area;
    }

    template<typename T>
    InterfaceGeom<T>::InterfaceGeom(GeomSurf<T>* geom, int n_marker, int n_id, T n_diam) :
      GeomSurf<T>(), neighb_marker(n_marker), neighb_id(n_id), neighb_diam(n_diam)
    {
        // Let this class expose the standard Geom interface.
        this->elem_marker = geom->elem_marker;
        this->isurf = geom->isurf;
        memcpy(this->x, geom->x, geom->np * sizeof(T));
        memcpy(this->y, geom->y, geom->np * sizeof(T));
        memcpy(this->tx, geom->tx, geom->np * sizeof(T));
        memcpy(this->ty, geom->ty, geom->np * sizeof(T));
        memcpy(this->nx, geom->nx, geom->np * sizeof(T));
        memcpy(this->ny, geom->ny, geom->np * sizeof(T));
        this->orientation = geom->orientation;
        this->wrapped_geom = geom;
      }

    template<>
    InterfaceGeom<Hermes::Ord>::InterfaceGeom(GeomSurf<Hermes::Ord>* geom, int n_marker, int n_id, Hermes::Ord n_diam) : GeomSurf<Hermes::Ord>()
    {
      this->wrapped_geom = geom;
    }

    template<typename T>
    void InterfaceGeom<T>::free()
    {
      delete wrapped_geom;
    }

    template<typename T>
    void InterfaceGeom<T>::free_ord()
    {
      delete wrapped_geom;
    }

    template<>
    void InterfaceGeom<Hermes::Ord>::free()
    {
      delete wrapped_geom;
    }

    template<typename T>
    int InterfaceGeom<T>::get_neighbor_marker() const
    {
      return neighb_marker;
    }

    template<typename T>
    int InterfaceGeom<T>::get_neighbor_id() const
    {
      return neighb_id;
    }

    template<typename T>
    T InterfaceGeom<T>::get_neighbor_diam() const
    {
      return neighb_diam;
    }

    GeomVol<double>* init_geom_vol(RefMap *rm, const int order)
    {
      GeomVol<double>* e = new GeomVol<double>;
      init_geom_vol_allocated(*e, rm, order);
      return e;
    }

    void init_geom_vol_allocated(GeomVol<double>& geom, RefMap *rm, const int order)
    {
      Element* element = rm->get_active_element();
      ElementMode2D mode = element->get_mode();
      geom.id = element->id;
      geom.elem_marker = element->marker;
      Quad2D* quad = rm->get_quad_2d();
      unsigned char np = quad->get_num_points(order, mode);
      memcpy(geom.x, rm->get_phys_x(order), np * sizeof(double));
      memcpy(geom.y, rm->get_phys_y(order), np * sizeof(double));
    }

    GeomSurf<double>* init_geom_surf(RefMap *rm, int isurf, int marker, const int order, double3*& tan)
    {
      GeomSurf<double>* e = new GeomSurf<double>;
      init_geom_surf_allocated(*e, rm, isurf, marker, order, tan);
      return e;
    }

    void init_geom_surf_allocated(GeomSurf<double>& geom, RefMap *rm, int isurf, int marker, const int order, double3*& tan)
    {
      Element* element = rm->get_active_element();
      ElementMode2D mode = element->get_mode();

      geom.edge_marker = marker;
      geom.elem_marker = element->marker;
      geom.isurf = isurf;

      tan = rm->get_tangent(isurf, order);

      Quad2D* quad = rm->get_quad_2d();
      unsigned char np = quad->get_num_points(order, mode);
      geom.np = np;

      memcpy(geom.x, rm->get_phys_x(order), np * sizeof(double));
      memcpy(geom.y, rm->get_phys_y(order), np * sizeof(double));

      for (int i = 0; i < np; i++)
      {
        geom.tx[i] = tan[i][0];  geom.ty[i] = tan[i][1];
        geom.nx[i] = tan[i][1];  geom.ny[i] = -tan[i][0];
      }
      geom.orientation = rm->get_active_element()->get_edge_orientation(isurf);
    }

    Func<double>* init_fn(PrecalcShapeset *fu, RefMap *rm, const int order)
    {
      Func<double>* u = preallocate_fn<double>();

      init_fn_preallocated(u, fu, rm, order);

      return u;
    }

    template<typename Scalar>
    Func<Scalar>* init_fn(MeshFunction<Scalar>* fu, const int order)
    {
      Func<Scalar>* u = preallocate_fn<Scalar>();

      init_fn_preallocated(u, fu, order);

      return u;
    }

    template<typename Scalar>
    Func<Scalar>* preallocate_fn(pj_pool_t* memoryPool)
    {
      if (memoryPool)
        return (Func<Scalar>*)pj_pool_alloc(memoryPool, sizeof(Func<Scalar>));
      else
        return new Func<Scalar>();
    }

    void init_fn_preallocated(Func<double>* u, PrecalcShapeset *fu, RefMap *rm, const int order)
    {
      SpaceType space_type = fu->get_space_type();

#ifdef H2D_USE_SECOND_DERIVATIVES
      if (space_type == HERMES_H1_SPACE || space_type == HERMES_L2_SPACE)
        fu->set_quad_order(order, H2D_FN_ALL);
      else
        fu->set_quad_order(order);
#else
      fu->set_quad_order(order);
#endif

      int nc = fu->get_num_components();
      unsigned char np = fu->get_quad_2d()->get_num_points(order, fu->get_active_element()->get_mode());
      u->np = np;
      u->nc = nc;

      // H1 & L2 space.
      if (space_type == HERMES_H1_SPACE || space_type == HERMES_L2_SPACE)
      {
        const double *fn = fu->get_fn_values();
        const double *dx = fu->get_dx_values();
        const double *dy = fu->get_dy_values();

#ifdef H2D_USE_SECOND_DERIVATIVES
        const double *dxx = fu->get_dxx_values();
        const double *dxy = fu->get_dxy_values();
        const double *dyy = fu->get_dyy_values();
#endif

        if (rm->is_jacobian_const())
        {
          double const_inv_ref_map00 = rm->get_const_inv_ref_map()[0][0][0];
          double const_inv_ref_map01 = rm->get_const_inv_ref_map()[0][0][1];
          double const_inv_ref_map10 = rm->get_const_inv_ref_map()[0][1][0];
          double const_inv_ref_map11 = rm->get_const_inv_ref_map()[0][1][1];
          for (int i = 0; i < np; i++)
          {
            u->val[i] = fn[i];
            u->dx[i] = (dx[i] * const_inv_ref_map00 + dy[i] * const_inv_ref_map01);
            u->dy[i] = (dx[i] * const_inv_ref_map10 + dy[i] * const_inv_ref_map11);
          }
#ifdef H2D_USE_SECOND_DERIVATIVES
          double3x2 *mm;
          mm = rm->get_second_ref_map(order);
          for (int i = 0; i < np; i++, mm++)
          {
            u->val[i] = fn[i];
            u->dx[i] = (dx[i] * const_inv_ref_map00 + dy[i] * const_inv_ref_map01);
            u->dy[i] = (dx[i] * const_inv_ref_map10 + dy[i] * const_inv_ref_map11);

            double axx = (Hermes::sqr(const_inv_ref_map00) + Hermes::sqr(const_inv_ref_map10));
            double ayy = (Hermes::sqr(const_inv_ref_map01) + Hermes::sqr(const_inv_ref_map11));
            double axy = 2.0 * (const_inv_ref_map00 * const_inv_ref_map01 + const_inv_ref_map10 * const_inv_ref_map11);
            double ax = (*mm)[0][0] + (*mm)[2][0];
            double ay = (*mm)[0][1] + (*mm)[2][1];
            u->laplace[i] = (dx[i] * ax + dy[i] * ay + dxx[i] * axx + dxy[i] * axy + dyy[i] * ayy);
          }
#endif
        }
        else
        {
          double2x2 *m = rm->get_inv_ref_map(order);

#ifdef H2D_USE_SECOND_DERIVATIVES
          double3x2 *mm;
          mm = rm->get_second_ref_map(order);
          for (int i = 0; i < np; i++, m++, mm++)
          {
            u->val[i] = fn[i];
            u->dx[i] = (dx[i] * (*m)[0][0] + dy[i] * (*m)[0][1]);
            u->dy[i] = (dx[i] * (*m)[1][0] + dy[i] * (*m)[1][1]);

            double axx = (Hermes::sqr((*m)[0][0]) + Hermes::sqr((*m)[1][0]));
            double ayy = (Hermes::sqr((*m)[0][1]) + Hermes::sqr((*m)[1][1]));
            double axy = 2.0 * ((*m)[0][0] * (*m)[0][1] + (*m)[1][0] * (*m)[1][1]);
            double ax = (*mm)[0][0] + (*mm)[2][0];
            double ay = (*mm)[0][1] + (*mm)[2][1];
            u->laplace[i] = (dx[i] * ax + dy[i] * ay + dxx[i] * axx + dxy[i] * axy + dyy[i] * ayy);
          }
#else
          for (int i = 0; i < np; i++, m++)
          {
            u->val[i] = fn[i];
            u->dx[i] = (dx[i] * (*m)[0][0] + dy[i] * (*m)[0][1]);
            u->dy[i] = (dx[i] * (*m)[1][0] + dy[i] * (*m)[1][1]);
          }
#endif
        }
      }
      // Hcurl space.
      else if (space_type == HERMES_HCURL_SPACE)
      {
        const double *fn0 = fu->get_fn_values(0);
        const double *fn1 = fu->get_fn_values(1);
        const double *dx1 = fu->get_dx_values(1);
        const double *dy0 = fu->get_dy_values(0);
        if (rm->is_jacobian_const())
        {
          double const_inv_ref_map00 = rm->get_const_inv_ref_map()[0][0][0];
          double const_inv_ref_map01 = rm->get_const_inv_ref_map()[0][0][1];
          double const_inv_ref_map10 = rm->get_const_inv_ref_map()[0][1][0];
          double const_inv_ref_map11 = rm->get_const_inv_ref_map()[0][1][1];
          double determinant = (const_inv_ref_map00 * const_inv_ref_map11 - const_inv_ref_map10 * const_inv_ref_map01);
          for (int i = 0; i < np; i++)
          {
            u->val0[i] = (fn0[i] * const_inv_ref_map00 + fn1[i] * const_inv_ref_map01);
            u->val1[i] = (fn0[i] * const_inv_ref_map10 + fn1[i] * const_inv_ref_map11);
            u->curl[i] = determinant * (dx1[i] - dy0[i]);
          }
        }
        else
        {
          double2x2* m = rm->get_inv_ref_map(order);
          for (int i = 0; i < np; i++, m++)
          {
            u->val0[i] = (fn0[i] * (*m)[0][0] + fn1[i] * (*m)[0][1]);
            u->val1[i] = (fn0[i] * (*m)[1][0] + fn1[i] * (*m)[1][1]);
            u->curl[i] = ((*m)[0][0] * (*m)[1][1] - (*m)[1][0] * (*m)[0][1]) * (dx1[i] - dy0[i]);
          }
        }
      }
      // Hdiv space.
      else if (space_type == HERMES_HDIV_SPACE)
      {
        const double *fn0 = fu->get_fn_values(0);
        const double *fn1 = fu->get_fn_values(1);
        const double *dx0 = fu->get_dx_values(0);
        const double *dy1 = fu->get_dy_values(1);
        if (rm->is_jacobian_const())
        {
          double const_inv_ref_map00 = rm->get_const_inv_ref_map()[0][0][0];
          double const_inv_ref_map01 = rm->get_const_inv_ref_map()[0][0][1];
          double const_inv_ref_map10 = rm->get_const_inv_ref_map()[0][1][0];
          double const_inv_ref_map11 = rm->get_const_inv_ref_map()[0][1][1];
          double determinant = (const_inv_ref_map00 * const_inv_ref_map11 - const_inv_ref_map10 * const_inv_ref_map01);

          for (int i = 0; i < np; i++)
          {
            u->val0[i] = (fn0[i] * const_inv_ref_map11 - fn1[i] * const_inv_ref_map10);
            u->val1[i] = (-fn0[i] * const_inv_ref_map01 + fn1[i] * const_inv_ref_map00);
            u->div[i] = determinant * (dx0[i] + dy1[i]);
          }
        }
        else
        {
          double2x2* m = rm->get_inv_ref_map(order);
          for (int i = 0; i < np; i++, m++)
          {
            u->val0[i] = (fn0[i] * (*m)[1][1] - fn1[i] * (*m)[1][0]);
            u->val1[i] = (-fn0[i] * (*m)[0][1] + fn1[i] * (*m)[0][0]);
            u->div[i] = ((*m)[0][0] * (*m)[1][1] - (*m)[1][0] * (*m)[0][1]) * (dx0[i] + dy1[i]);
          }
        }
      }
      else
        throw Hermes::Exceptions::Exception("Wrong space type - space has to be either H1, Hcurl, Hdiv or L2");
    }

    template<typename Scalar>
    void init_fn_preallocated(Func<Scalar>* u, MeshFunction<Scalar>* fu, const int order)
    {
      // Sanity checks.
      if (fu == nullptr)
        throw Hermes::Exceptions::Exception("nullptr MeshFunction in Func<Scalar>*::init_fn().");
      if (fu->get_mesh() == nullptr)
        throw Hermes::Exceptions::Exception("Uninitialized MeshFunction used.");

      Quad2D* quad = fu->get_quad_2d();
#ifdef H2D_USE_SECOND_DERIVATIVES
      Solution<Scalar>* sln = dynamic_cast<Solution<Scalar>*>(fu);
      if (sln)
      {
        if ((sln->get_space_type() == HERMES_H1_SPACE || sln->get_space_type() == HERMES_L2_SPACE) && sln->get_type() != HERMES_EXACT)
          fu->set_quad_order(order, H2D_FN_ALL);
        else
          fu->set_quad_order(order);
      }
      else
        fu->set_quad_order(order);
#else
      fu->set_quad_order(order);
#endif
      int nc = fu->get_num_components();
      unsigned char np = quad->get_num_points(order, fu->get_active_element()->get_mode());
      u->np = np;
      u->nc = nc;

      if (u->nc == 1)
      {
        memcpy(u->val, fu->get_fn_values(), np * sizeof(Scalar));
        memcpy(u->dx, fu->get_dx_values(), np * sizeof(Scalar));
        memcpy(u->dy, fu->get_dy_values(), np * sizeof(Scalar));

#ifdef H2D_USE_SECOND_DERIVATIVES
        if (sln && (sln->get_space_type() == HERMES_H1_SPACE || sln->get_space_type() == HERMES_L2_SPACE) && sln->get_type() != HERMES_EXACT)
        {
          Scalar *dxx = fu->get_dxx_values();
          Scalar *dyy = fu->get_dyy_values();
          for (int i = 0; i < np; i++)
            u->laplace[i] = dxx[i] + dyy[i];
        }
#endif
      }
      else
      {
        memcpy(u->val0, fu->get_fn_values(0), np * sizeof(Scalar));
        memcpy(u->val1, fu->get_fn_values(1), np * sizeof(Scalar));

        const Scalar *dx1 = fu->get_dx_values(1);
        const Scalar *dy0 = fu->get_dy_values(0);
        for (int i = 0; i < np; i++)
          u->curl[i] = dx1[i] - dy0[i];

        const Scalar *dx0 = fu->get_dx_values(0);
        const Scalar *dy1 = fu->get_dy_values(1);
        for (int i = 0; i < np; i++)
          u->div[i] = dx0[i] + dy1[i];
      }
    }

    template<typename Scalar>
    void init_fn_preallocated(Func<Scalar>* u, UExtFunction<Scalar>* fu, Func<Scalar>** ext, Func<Scalar>** u_ext, const int order, GeomBasic<double>* geometry, ElementMode2D mode)
    {
      // Sanity check.
      if (fu == nullptr)
        throw Hermes::Exceptions::Exception("nullptr UExtFunction in Func<Scalar>*::init_fn().");

      Quad2D* quad = &g_quad_2d_std;
      unsigned char np = quad->get_num_points(order, mode);

      fu->value(np, ext, u_ext, u, geometry);
    }


    template<typename Scalar>
    Func<Scalar>* init_zero_fn(ElementMode2D mode, int order, Quad2D* quad, int nc)
    {
      if (quad == nullptr)
        quad = &g_quad_2d_std;

      unsigned char np = quad->get_num_points(order, mode);
      Func<Scalar>* u = new Func<Scalar>(np, nc);

      if (nc == 1)
      {
        memset(u->val, 0, np * sizeof(Scalar));
        memset(u->dx, 0, np * sizeof(Scalar));
        memset(u->dy, 0, np * sizeof(Scalar));
      }
      else if (nc == 2)
      {
        memset(u->val0, 0, np * sizeof(Scalar));
        memset(u->val1, 0, np * sizeof(Scalar));
        memset(u->curl, 0, np * sizeof(Scalar));
        memset(u->div, 0, np * sizeof(Scalar));
      }
      return u;
    }

    template<typename Scalar>
    Func<Scalar>* init_fn(UExtFunction<Scalar>* fu, Func<Scalar>** ext, Func<Scalar>** u_ext, const int order, GeomBasic<double>* geometry, ElementMode2D mode)
    {
      int nc = 1;
      Quad2D* quad = &g_quad_2d_std;

      unsigned char np = quad->get_num_points(order, mode);
      Func<Scalar>* u = new Func<Scalar>(np, nc);

      // Sanity check.
      if (fu == nullptr)
        throw Hermes::Exceptions::Exception("nullptr UExtFunction in Func<Scalar>*::init_fn().");

      fu->value(np, ext, u_ext, u, geometry);

      return u;
    }

    template HERMES_API Func<double>* init_fn(MeshFunction<double>* fu, const int order);
    template HERMES_API Func<std::complex<double> >* init_fn(MeshFunction<std::complex<double> >* fu, const int order);

    template HERMES_API Func<double>* preallocate_fn(pj_pool_t* memoryPool);
    template HERMES_API Func<std::complex<double> >* preallocate_fn(pj_pool_t* memoryPool);

    template HERMES_API void init_fn_preallocated(Func<double>* u, MeshFunction<double>* fu, const int order);
    template HERMES_API void init_fn_preallocated(Func<std::complex<double> >* u, MeshFunction<std::complex<double> >* fu, const int order);

    template HERMES_API void init_fn_preallocated(Func<double>* u, UExtFunction<double>* fu, Func<double>** ext, Func<double>** u_ext, const int order, GeomBasic<double>* geometry, ElementMode2D mode);
    template HERMES_API void init_fn_preallocated(Func<std::complex<double> >* u, UExtFunction<std::complex<double> >* fu, Func<std::complex<double> >** ext, Func<std::complex<double> >** u_ext, const int order, GeomBasic<double>* geometry, ElementMode2D mode);


    template HERMES_API Func<double>* init_zero_fn(ElementMode2D mode, int order, Quad2D* quad_2d, int nc);
    template HERMES_API Func<std::complex<double> >* init_zero_fn(ElementMode2D mode, int order, Quad2D* quad_2d, int nc);

    template HERMES_API Func<double>* init_fn(UExtFunction<double>* fu, Func<double>** ext, Func<double>** u_ext, const int order, GeomBasic<double>* geometry, ElementMode2D mode);
    template HERMES_API Func<std::complex<double> >* init_fn(UExtFunction<std::complex<double> >* fu, Func<std::complex<double> >** ext, Func<std::complex<double> >** u_ext, const int order, GeomBasic<double>* geometry, ElementMode2D mode);

    template class HERMES_API DiscontinuousFunc<double>;
    template class HERMES_API DiscontinuousFunc<std::complex<double> >;
    template class HERMES_API GeomVol<double>;
    template class HERMES_API GeomSurf<double>;
    template class HERMES_API GeomBasic<double>;
    template class HERMES_API InterfaceGeom<Hermes::Ord>;
    template class HERMES_API InterfaceGeom<double>;
  }
}
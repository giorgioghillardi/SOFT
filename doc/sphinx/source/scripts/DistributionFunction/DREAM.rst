.. _module-distribution-dream:

(dream)
-----------
This module provides a way for the user to load a distribution function from
an output file generated by the kinetic solver
`DREAM <https://github.com/chalmersplasmatheory/DREAM>`_. The module is capable
of loading both hot electron- and runaway distribution functions.

Summary of options
^^^^^^^^^^^^^^^^^^

+-------------------------------+------------------------------------------------------------------------------+
| **Option**                    | **Description**                                                              |
+-------------------------------+------------------------------------------------------------------------------+
| :option:`dream distribution`  | Name of distribution function to load (``f_hot`` or ``f_re``).               |
+-------------------------------+------------------------------------------------------------------------------+
| :option:`dream flippitchsign` | If true, flips the sign of the particle pitch.                               |
+-------------------------------+------------------------------------------------------------------------------+
| :option:`dream interptype`    | Interpolation method to use.                                                 |
+-------------------------------+------------------------------------------------------------------------------+
| :option:`dream logarithmize`  | Whether or not to interpolate in the logarithm of the distribution function. |
+-------------------------------+------------------------------------------------------------------------------+
| :option:`dream name`          | Name of file containing DREAM distribution function.                         |
+-------------------------------+------------------------------------------------------------------------------+

Example configuration
^^^^^^^^^^^^^^^^^^^^^
The following example configures a DREAM distribution function::

   @DistributionFunction ourDistributionFunction (dream) {
       name = "/path/to/distribution.h5";
       distribution = "f_hot";
   }

All options
^^^^^^^^^^^

.. program:: dream

.. option:: distribution

   :Default value: ``f_re``, if it exists in the output file, otherwise ``f_hot``.
   :Allowed values: ``f_hot`` or ``f_re``.

   Specifies the name of the distribution function to load. DREAM can simulate
   the evolution of two connected distribution functions simultaneously and this
   option specifies which of the two distribution functions to use. Generally,
   synchrotron radiation originates in the high-energy region of the
   distribution which is represented by ``f_re``. Hence, ``f_re`` is used by
   default unless this option is explicitly set otherwise. Some simulations
   keep the entire distribution function on a single grid, in which case SOFT
   will pick the only available distribution function.

.. option:: flippitchsign

   :Default value: ``no``
   :Allowed values: ``yes`` or ``no``

   If ``yes``, transforms :math:`f(p,\xi)\to f(p,-\xi)`. This is useful in case
   the distribution function is defined in an inconsistent way (i.e. if a
   positive parallel electric field accelerates particles in the anti-parallel
   direction).

.. option:: interptype

   :Default value: ``cubic``
   :Allowed values: ``cubic`` or ``linear``

   SOFT interpolates in the given distribution function to evaluate it at
   arbitrary points on the phase space grid. A linear interpolation scheme is
   always used to interpolate in the radial coordinate, but interpolation in
   the momentum coordinates (:math:`p` and :math:`\xi`) can either be done using
   bi-linear or bi-cubic splines.

.. option:: logarithmize

   :Default value: ``no``
   :Allowed values: ``yes`` or ``no``

   If ``yes``, interpolates in the logarithm of the distribution function
   instead of in the distribution function directly. This can aid in fitting
   sharply decaying ditsribution functions.

.. option:: name

   :Default value: Nothing
   :Allowed values: Any valid file name

   Name of the file containing the distribution function.


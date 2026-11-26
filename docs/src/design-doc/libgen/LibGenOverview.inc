``lib-gen`` is a C++ library of general purpose, cross cutting design
elements. Nothing in it knows about craps, and no other project library is
below it. Every other library (``lib-craps``, ``lib-controller``,
``lib-cui``) may use it. It depends only on the third party libraries for
console color (``rang``), YAML, ``zeus::expected`` and ``fmt``.

How the game uses these elements is in :ref:`Architecture`. This section
describes the elements themselves, as CRC cards, so that they can be used in
another application.

Other Classes in lib-gen
------------------------

The smaller classes are not given cards of their own.

.. list-table::
   :header-rows: 1
   :widths: 24 76

   * - Class
     - Purpose
   * - ``Money``
     - An amount of money held exactly, in whole cents, with formatting
       manipulators and YAML conversion. See :ref:`Model Overview`.
   * - ``EdgeEvent``
     - Lets one thread wait for another to signal. A signal is missed if no
       thread is waiting when it is made.
   * - ``SignalHandler``
     - Blocks the termination signals in every thread, and lets the main
       thread wait for one to arrive. See :ref:`Thread Inventory`.
   * - ``FileUtils``
     - Opens a file or throws, and turns a name into a safe file name:
       lower case, with hyphens for spaces.
   * - ``Uuid``
     - Makes a unique id as a string.
   * - ``Timepoint``
     - A point in time with conversions and operators.
   * - ``BuildInfo``
     - What is known about how the program was built.
   * - ``ReturnCode``
     - The ``Success`` and ``Fail`` codes of the call-chain style functions.

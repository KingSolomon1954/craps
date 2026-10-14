*******************
Design of |AppName|
*******************

This document describes the design of |AppName| version |version|.

.. include:: /include/introduction.rst

This document is primarily for developers maintaining the
application itself, however, other interested developers and
engineers may find it useful.

.. contents:: Table of Contents
   :depth: 1
   :local:

User Story
==========

As a user of |AppName|, I want to:

* TBS
* TBS
* TBS

Design Goals
============

Important application design goals and constraints:

* Goal 1
* Goal 2

Application Context
===================

.. admonition:: Instructions

   In this section describe the application's view of its processing
   context. Create or modify the following diagram to agree with your
   app. Then eliminate this admonition from the design-doc.rst file.

The |AppName| design is a single process application with multiple
threads written specifically for Linux. C++ is the language choice.
   
The following figure shows |AppName|'s view of its processing
environment.

.. figure:: /images/pub/ContextApplication.png
    :align: center
    :alt: Application Context Diagram

    **Figure:** Application Context Diagram

The design conforms the Model/View/Controller design paradigm. There is
the CUI and GUI making up the View, the Controller, and the Craps Engine
making up the Model. Implementation-wise, we have:

* Model - ``lib-craps``
* View - ``lib-cui`` / ``lib-gui``
* Controller - ``lib-controller``
  
There is no database, just a bunch of persistent files.
The application performs no networking.
    
Startup/Shutdown
----------------

.. include:: StartupBehavior.rst

Threads
-------

TBS

Memory Management
-----------------

TBS

Global Data
-----------

TBS

GameEvents

State Management
----------------

TBS

.. figure:: /images/pub/StateDiagram.png
   :alt: State Diagram

   **Figure:** State Diagram

Processing Flow
---------------

<Describe processing flow to agree with diagram>

.. figure:: /images/pub/ProcessingFlow.png
    :alt: Processing Flow

    **Figure:** Processing Flow

main.cpp
--------

TBS

Persistence
-----------

TBS

CRC Cards
---------

CRC cards is succinct methodology for object oriented designs where CRC
stands for classes, responsibilities, and collaborations. CRC cards are
used to discover, collect and document class information. Using CRC
cards, each card represents a class and its properties, and is shown in
its own paragraph. See `Extreme Programming CRC Cards
<http://www.extremeprogramming.org/rules/crccards.html>`_ or `wikipedia
<https://en.wikipedia.org/wiki/Class-responsibility-collaboration_card>`_.

Each design section below presents their important classes using CRC cards.

Some sections will present a UML class as well. Something like these.

.. figure:: /images/pub/StaticClassDiagram.png
   :alt: Static Class Diagram

   **Figure:** Static Class Diagram

Craps Engine
============

Players
-------

TBS

Tables
------

TBS

Betting
-------

TBS

Statistics
----------

TBS

Controller
==========

Interfaces
----------

TBS

Event Handling
--------------

TBS

Configuration
-------------

Command Line

Configuration Files

Environment Vars

TBS

Console User Interface
======================

TBS

OperationManager CRC Card
-------------------------

.. include:: OperationManager.rst


CuiMain CRC Card
----------------

TBS

ScreenCrapsTable CRC Card
-------------------------

TBS

Graphical User Interface
========================

TBS

Physics engine
--------------

TBS

Library General
===============

lib-gen is a C++ library providing general purpose cross cutting
design elements that most any application can use.

Multi-Layer Configuration
-------------------------

TBS

Error Handling
--------------

TBS

Error Passing

Exceptions

ErrorDialogs


.. figure:: /images/pub/ErrorSeq.png
   :alt: Error Sequence Diagram

   **Figure:** Error Sequence Diagram

EventManager
------------

TBS

Logger
------

TBS

Timers
------

TBS

Security
--------

TBS

Quality Attributes
==================

This section identifies the quality attributes that are most important
in this design. The first group of elements are considered the higher
priority concerns for the design of this application.

.. admonition:: Instructions

   Modify this section appropriate for your application.

| *Correctness*
| *Usability*
| *Readability*
| *Maintainability*
| *Completeness*
| *Performance*

The following quality attributes are less of a concern in this design:

| *Timeliness*
| *Reliability*
| *Robustness*
| *Efficiency*
| *Capacity*
| *Scalability*
| *Compatibility*
| *Interoperability*
| *Integrity/Security*
| *Adaptability*
| *Configurability*
| *Accuracy*
| *Repeatability*
| *Buildability*
| *Flexibility*
| *Portability*
| *Reusability*
| *Testability*
| *Understandability*

Collateral Design
=================

Language
--------

Implementation language is C++.

Building
--------

Uses the standard project build architecture.

COTS
----

Links with COTS gRPC community library.

Packaging
---------

Packaged as an RPM.

Automated Unit Testing
----------------------

Classes are unit tested using the COTS doctest framework.

Automated Component Testing
---------------------------

TBS

Automated Integration Testing
-----------------------------

TBS

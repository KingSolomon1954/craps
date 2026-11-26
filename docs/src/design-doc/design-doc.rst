*******************
Design of |AppName|
*******************

This document describes the design of |AppName| version |version|.

.. include:: /include/introduction.rst

This document is primarily for developers maintaining the
application itself, however, other interested developers and
engineers may find it useful.

.. contents:: Table of Contents
   :depth: 2
   :local:

Introduction
============

User Story
----------

As a person who likes craps, but not the risk of losing real money, I want
to sit at a casino style craps table on my own Linux computer and play it
the way it is played in a casino.

As a user of |AppName|, I want to:

* Place any of the bets found on a casino table: Pass Line, Don't Pass,
  Come, Don't Come, Place, Buy, Lay, Field, Hardway and the proposition
  bets, with odds behind the line bets, and have the table pay them by the
  real rules.
* Learn the game without risking a dollar, because the table shows the limits
  for a bet and tells me why when it refuses one.
* Try a betting system and see how it does over many rolls, with statistics
  for me, for the table and for the dice.
* Save time on the bets I make over and over, with Quick Bets, Automatic Odds
  and Bet Flags, and take back a mistake with Undo while betting is open.
* Share the table with buddy players, and have several players of my own,
  each with a bankroll and a history that are there the next time I play.
* Choose a table with limits and odds that suit my bankroll.
* Play from the keyboard in a terminal, with the dice animated and a history
  of the rolls in view.

Design Goals
------------

The important design goals and constraints are these.

* **Casino rules, as data.** The rules of a table (limits, odds allowed,
  payouts and commission) are data in the table's file, not code, so a table
  with different rules is a new file. See :ref:`The Rules Are Data`.
* **An engine that does not know the screen.** The Craps Engine knows
  nothing of the View, so a graphical interface can be added beside the
  console interface and use the same engine and Controller. See
  :ref:`MVC and the Libraries`.
* **A screen that is never held up.** The console interface runs on its own
  thread, and a request from the screen is answered by an event instead of a
  call that waits. See :ref:`Threads and Processing Flow`.
* **Refuse, explain and change nothing.** A bet or other request that is not
  allowed is refused whole, with a reason the person can read, and leaves
  the game as it was.
* **Persistent and simple.** Players and tables are kept in YAML files that a
  person can read. There is no database and no network. See
  :ref:`Persistence and Configuration`.
* **Testable in pieces.** The engine and the Controller run without a screen,
  and unit tests play whole rolls and whole sessions.
* **Single process, built for Linux.** The language is C++20 and the
  console interface uses ncurses.
* **Documentation built with the code,** so that it stays true to it.

Quality Attributes
------------------

The quality attributes that are most important in this design are:

| *Correctness*
| *Usability*
| *Readability*
| *Maintainability*
| *Completeness*
| *Performance*

How to Read This Document
-------------------------

The document has three kinds of sections:

* **Architecture** covers the concerns that cut across the whole
  application, such as threads, events, startup and persistence.
* **Model**, **Controller** and **View** cover the structure of each layer
  and present its significant classes.
* **Features** follow one feature at a time through all of the layers.

CRC cards are a succinct methodology for object oriented designs where CRC
stands for classes, responsibilities, and collaborations. Each layer section
presents its significant classes using abbreviated CRC cards that show the
major purpose of a class, not its data or function signatures. See `Extreme
Programming CRC Cards <http://www.extremeprogramming.org/rules/crccards.html>`_
or `wikipedia
<https://en.wikipedia.org/wiki/Class-responsibility-collaboration_card>`_.

Some sections will present a UML class diagram as well. Something like this.

.. figure:: /images/pub/StaticClassDiagram.png
   :alt: Static Class Diagram

   **Figure:** Static Class Diagram

Architecture
============

Application Context
-------------------

The |AppName| design is a single process application with multiple
threads written specifically for Linux. C++ is the language choice.

The following figure shows |AppName|'s view of its processing
environment.

.. figure:: /images/pub/ContextApplication.png
    :align: center
    :alt: Application Context Diagram

    **Figure:** Application Context Diagram

There is no database, just a bunch of persistent files.
The application performs no networking.

MVC and the Libraries
---------------------

The design conforms the Model/View/Controller design paradigm. There is
the CUI and GUI making up the View, the Controller, and the Craps Engine
making up the Model. Implementation-wise, we have:

* Model - ``lib-craps``
* View - ``lib-cui`` / ``lib-gui``
* Controller - ``lib-controller``
* General purpose library - ``lib-gen``

Threads and Processing Flow
---------------------------

.. include:: architecture/Threads.inc

Events
------

.. include:: architecture/Events.inc

Roll Animation Synchronization
------------------------------

.. include:: architecture/RollAnimationSync.inc

State and Global Data
---------------------

.. include:: architecture/State.inc

Startup, Shutdown and Directory Layout
--------------------------------------

.. include:: architecture/StartupBehavior.inc

Persistence and Configuration
-----------------------------

.. include:: architecture/Persistence.inc

Error Handling, Logging and Timers
----------------------------------

.. include:: architecture/ErrorsLoggingTimers.inc

Memory Management and Security
------------------------------

.. include:: architecture/MemorySecurity.inc

Model
=====

Model Overview
--------------

.. include:: model/ModelOverview.inc

Model CRC Cards
---------------

.. include:: model/ModelCards.inc

Controller
==========

The Controller is in ``lib-controller``.

Controller Overview
-------------------

The Controller sits between the View and the Craps Engine. It receives the
View's requests, carries them out through the engine, owns the files the
game reads and writes, and keeps what the engine does not need to know:
the players' preferences, undo history and configuration.

.. list-table::
   :header-rows: 1
   :widths: 24 76

   * - Group
     - Classes
   * - Application
     - ``GameMain``, ``Gbl`` (the globals)
   * - Requests and reading
     - ``EventHandler``, ``GameEvents``, ``CrapsReaders``
   * - Players and tables
     - ``PlayerManager``, ``TableManager``, ``PlayerManifest``,
       ``TableManifest``, ``YamlFile``
   * - Bet help
     - ``UndoManager``, ``BetExecutor``, ``BetSpec``, ``PlayerPrefs`` with
       ``QuickBets``, ``AutoOdds`` and ``LastFill``, and ``BetFlags``
   * - Configuration and startup
     - ``ConfigManager`` with its layer loaders, ``ConfigSeed``,
       ``StartChoices``, ``UserState``

The Controller knows nothing of the screen. It does not call the View. It
answers with events.

Controller CRC Cards
--------------------

The cards below are for the significant classes. The way each feature uses
them together is in :ref:`Features`.

.. include:: controller/Controller.inc

View
====

The View is the Console User Interface in ``lib-cui``, and the Graphical
User Interface in ``lib-gui``.

Console User Interface
----------------------

.. include:: view/CuiOverview.inc

CUI Core CRC Cards
------------------

.. include:: view/CuiCards.inc

OperationManager CRC Card
-------------------------

.. include:: view/OperationManager.inc

Roll History and Roll Log
-------------------------

.. include:: view/RollHistory.inc

CUI Display CRC Cards
---------------------

.. include:: view/CuiPanels.inc

Graphical User Interface
------------------------

TBS

Physics Engine
^^^^^^^^^^^^^^

TBS

Features
========

Each feature is followed through the Model, Controller and View.

Betting
-------

.. include:: features/Betting.inc

Bet Automation
--------------

.. include:: features/BetAutomation.inc

Bet Flags
---------

.. include:: features/BetFlags.inc

Undo of Bet Changes
-------------------

.. include:: features/Undo.inc

Players and Buddies
-------------------

.. include:: features/PlayersBuddies.inc

Table Switching
---------------

.. include:: features/TableSwitch.inc

Statistics
----------

TBS

Library General
===============

.. include:: libgen/LibGenOverview.inc

ErrorPass
---------

.. include:: libgen/ErrorPass.inc

EventManager
------------

.. include:: libgen/EventManager.inc

Logger
------

.. include:: libgen/Logger.inc

TimerManager
------------

.. include:: libgen/TimerManager.inc

MultiLayerConfig
----------------

.. include:: libgen/MultiLayerConfig.inc

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

TBS

Packaging
---------

TBS

Automated Unit Testing
----------------------

Classes are unit tested using the COTS doctest framework.

Automated Component Testing
---------------------------

TBS

Automated Integration Testing
-----------------------------

TBS

Roll Log Purpose
^^^^^^^^^^^^^^^^

Two parts of the CUI show past rolls: the one-line roll history under the
title bar, and the Deep Roll History popup. Both need the same facts about
each roll, including whether it was a seven out, a pass line winner or
established a point, because those pick the roll's color. The Craps Engine
remembers the dice of recent rolls but not what each roll meant, and those
facts arrive as separate events after the roll.

The CUI therefore keeps its own memory of rolls, ``RollLog``, and both
displays draw from it.

Roll Log Participants
^^^^^^^^^^^^^^^^^^^^^

* ``RollLog`` is a small singleton holding ``RollEntry`` values, newest
  first. An entry has the two dice and three flags: ``sevenOut``,
  ``passLineWinner`` and ``point``. It keeps at most ``Capacity`` entries
  (120) and drops the oldest.
* ``WindowRollHistory`` adds an entry for each new dice value, then marks
  the newest entry as the seven out, pass line winner and point events
  arrive. It formats and draws the entries in the format the person chose.
* ``DialogDeepRollHistory`` is a fixed-size popup showing the whole log as
  20 rows of 6 columns, newest at the top of the first column, reading down
  each column in turn. It always uses the most detailed format. It keeps
  nothing of its own and has no dependency on any menu: whatever offers it
  calls ``show()``.

Roll Log Rules
^^^^^^^^^^^^^^

* The log is the View's memory only. It holds what the CUI has seen since
  it started and is not saved. The engine's own recent rolls are not used,
  because they carry no flags.
* Entries are created and marked by the same work orders as every other
  roll result. They are therefore held and released with the dice
  animation (see Dice Roll Animation Synchronization), and the history
  never shows a roll before the dice have landed.
* The marks apply to the newest entry only and do nothing when the log is
  empty.
* Because the flags are stored per roll, changing the format redraws the
  entire history with its colors intact. Nothing has to be recomputed.
* ``Capacity`` is the number of cells in the popup. It is a fixed
  constant, not a user setting. The engine's ``TableManager::MaxRecentRolls``
  is kept at the same value.

Roll History Format Setting
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The format is a number from 1, the most compact, to 4, the most detailed:
``7 8 6``, ``7,8,6``, ``7(34) 8(44) 6(33)`` and ``7(3,4) 8(4,4) 6(3,3)``.
It follows the same path as the animation speed. The configuration key is
``rollHistory.format``. A choice made in the Control, Configure menu is sent
as ``ReqSetRollHistoryFormat`` and stored in the highest priority
configuration layer. The Controller answers with
``UslRollHistoryFormatChanged``, and the dispatcher redraws the screen. The
answer is needed because the request is handled on another thread, so
redrawing when the picker closes could happen before the new format is set.
``WindowRollHistory`` reads the setting each time it draws. The choice is
remembered in ``state.yaml`` at quit unless ``config.yaml`` states it.

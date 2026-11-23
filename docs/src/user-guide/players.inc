.. Conventions are the same as in statistics.inc: **bold** for the first use
.. of a defined term, *italic* for emphasis, ``literal`` for file names,
.. settings and commands, and Notes for things that commonly surprise people.

Players
=======

At |AppName| you play as one **player**. Your player has a name, a bank,
and statistics that are kept from one visit to the next. You can have as
many players as you like, and you can play as any of them, one at a time.

Other players can sit at the table with you. These are **buddy players**:
they come with the game, they bet on their own, and you can watch how they
do. A buddy player is not a player you can become; buddies are described
at the end of this section.

Everything in this section is in the *Player Menu*. Press ``C`` for
the Control menu, then ``p``.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Key
     - What it does
   * - ``l``
     - **List Players** shows the players you can play as, each with a short
       description. The one you are playing now is marked ``(you)``.
   * - ``s``
     - **Switch to Player** changes who you play as.
   * - ``r``
     - **Rename Player** changes the name of the player you are playing as.
   * - ``c``
     - **Create New Player** asks for a name and a short description, then
       makes a new player.
   * - ``d``
     - **Delete Player** deletes a player you are not playing as.
   * - ``b``
     - **List Buddies** shows the buddy players, with their descriptions,
       and which are at the table.
   * - ``j``
     - **Buddy Joins Table** seats a buddy player at the table.
   * - ``k``
     - **Buddy Leaves Table** sends a buddy player away from the table.

Choosing From a List
--------------------

Players are listed with their descriptions, like this:

.. code-block:: text

   ┌────────────────────────────────────────────┐
   │ Your Players                               │
   ├────────────────────────────────────────────┤
   │ Nathan   (you) | Primary player            │
   │ Howie          | Left-handed dice thrower  │
   ├────────────────────────────────────────────┤
   │ [. or esc or enter] Back                   │
   └────────────────────────────────────────────┘

A description that is too long for the screen is cut short with three
dots.

Where you pick a player from a list, use the up and down arrow keys and
press ``Enter``, or press the letter shown next to the name. ``Esc`` backs
out without doing anything.

Your First Player
-----------------

The first time you run |AppName| you are playing as a player named
*unnamed*. Use Rename Player to choose a name. A name can be up to 16
letters, digits, spaces and ordinary punctuation, and no two players can
have the same name. Capital and small letters count as the same: you
cannot have both *Howie* and *howie*.

.. note::

   Renaming changes only the name. Your bank and statistics stay with you.
   The player's file keeps its original name, for example ``unnamed.yaml``,
   so it is still found by ``config.yaml`` and ``state.yaml``, which refer
   to players by file name.

Creating and Switching
----------------------

Create New Player asks for a name and then for a short description, such as
*Left-handed dice thrower*. The description is optional: press ``Enter``
with nothing typed to skip it. A description can be up to 40 characters.
The new player starts with a fresh bank of $10,000 and no statistics. It
does *not* switch to the new player. Use
Switch to Player when you are ready to play as them.

When you switch, the player you were playing leaves the table and the new
player takes their seat. If the player you were playing has bets on the
table you are asked first. Those bets are not lost: they are taken off the
table and the money goes back to that player's bank.

.. note::

   Whenever a player leaves the table, for any reason, any bets they have
   on the table are returned to their bank. This is the same when you
   quit the game.

Deleting a Player
-----------------

Delete Player asks you to confirm. You cannot delete the player you are
playing as, so you always have at least one player. A deleted player's
file is not erased. It is moved into a ``trash`` folder inside the
``players`` folder in your data directory, and you can move it back by hand
if you change your mind. Because the player is also removed from
``players.yaml``, you would need to add that entry back as well.

Buddy Players
-------------

Buddy players sit at the table with you and make their own bets. Use
Buddy Joins Table and Buddy Leaves Table to change who is there. A buddy
who leaves has any bets returned and has their results saved. A table holds
at most six players, counting you, so a buddy cannot join a full table.

Buddy players cannot be renamed, deleted, or played as. If you want a
player you can control, use Create New Player.

If you would like the same buddies at the table every time you start, see
``startWithTheseBuddyPlayers`` in the section on configuring
|AppName|.

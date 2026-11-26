Memory Management
^^^^^^^^^^^^^^^^^

Ownership is held by one object, and the rest only point to it.

* ``GameMain`` creates and owns the globals, and releases them in reverse
  order when it ends.
* ``TableManager`` owns every ``CrapsTable``, with the file each was read
  from. A table the game has left is retired, not deleted. See
  :ref:`Table Switch Steps`.
* ``PlayerManager`` owns every ``Player`` along with its preferences and
  its file. A ``CrapsTable`` only points to the players seated at it, and a
  ``Player`` only points to the table it sits at. Neither owns the other.
* The View owns its windows, menus and dialogs. The Controller and the
  engine do not point into the View.
* Events are copied into the bus. An event is not shared between publisher
  and subscriber.

Smart pointers (``std::unique_ptr``) are used for ownership. Raw pointers
mean "not owned". The design makes no use of reference counting.

The program uses little memory, since there is a handful of players and
tables, and history is capped. A table keeps a limited number of its
sessions in its history, set by the ``table.maxSessions`` setting.

Security
^^^^^^^^

|AppName| runs as an ordinary user and has no network access. It reads and
writes files only in the user's own directories and in its temporary
files beside them. It holds no credentials, accounts or real money.
The user can edit the files in the user's directories, so the game reads
them without trusting them. A file that cannot be read is reported, or an
unusable entry is skipped, and the game still starts. The dice use a
standard random number generator, which is right for a game and not meant
for anything that needs unpredictable numbers.

MultiLayerConfig CRC Card
^^^^^^^^^^^^^^^^^^^^^^^^^

Settings from several places, read as one.

**Responsibilities**

* **Hold named layers.** Each layer is a map of keys to string values.
  The standard layer names are ``defaults``, ``files``, ``env`` and
  ``cmdline``. An application may add others.
* **Resolve a key.** The layers are searched from the last one added to the
  first, and the first layer that has the key wins. So the order the layers
  are added is the order of priority.
* **Convert on the way out.** ``getInt`` and ``getBool`` convert the text,
  and answer nothing if it does not convert. ``getBool`` accepts true,
  false, yes, no, on, off, 1 and 0 in any case.
* **Export** the resolved settings, one value for each key.
* **Be safe from any thread,** with a mutex.

**Collaborators**

* ``ConfigManager`` (Controller): adds the game's layers, including a
  session layer on top, and fills them. See :ref:`Persistence and
  Configuration`.

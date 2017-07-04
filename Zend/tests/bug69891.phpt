--TEST--
Bug #69891: Unexpected array comparison result
--FILE--
<?php

function fn1764319078()
{
    var_dump([1, 2, 3] <=> []);
    var_dump([] <=> [1, 2, 3]);
    var_dump([1] <=> [2, 3]);
}
fn1764319078();
--EXPECT--
int(1)
int(-1)
int(-1)

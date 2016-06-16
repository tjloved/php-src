--TEST--
Bug #69891: Unexpected array comparison result
--FILE--
<?php

function fn709271105()
{
    var_dump([1, 2, 3] <=> []);
    var_dump([] <=> [1, 2, 3]);
    var_dump([1] <=> [2, 3]);
}
fn709271105();
--EXPECT--
int(1)
int(-1)
int(-1)

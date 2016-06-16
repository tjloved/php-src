--TEST--
Operator precedence
--FILE--
<?php

function fn216233243()
{
    /* $Id$ */
    var_dump((object) 1 instanceof stdClass);
    var_dump(!(object) 1 instanceof Exception);
}
fn216233243();
--EXPECT--
bool(true)
bool(true)

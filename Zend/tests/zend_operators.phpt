--TEST--
Operator precedence
--FILE--
<?php

function fn1092672407()
{
    /* $Id$ */
    var_dump((object) 1 instanceof stdClass);
    var_dump(!(object) 1 instanceof Exception);
}
fn1092672407();
--EXPECT--
bool(true)
bool(true)

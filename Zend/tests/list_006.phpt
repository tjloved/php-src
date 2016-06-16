--TEST--
Testing nested list() with empty array
--FILE--
<?php

function fn1042151396()
{
    list($a, list($b, list(list($d)))) = array();
}
fn1042151396();
--EXPECTF--
Notice: Undefined offset: 0 in %s on line %d

Notice: Undefined offset: 1 in %s on line %d

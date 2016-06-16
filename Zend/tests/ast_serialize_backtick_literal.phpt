--TEST--
Serialization of backtick literal is incorrect
--INI--
zend.assertions=1
--FILE--
<?php

function fn1494205477()
{
    assert_options(ASSERT_WARNING);
    assert(false && `echo -n ""`);
}
fn1494205477();
--EXPECTF--
Warning: assert(): assert(false && `echo -n ""`) failed in %s%east_serialize_backtick_literal.php on line %d

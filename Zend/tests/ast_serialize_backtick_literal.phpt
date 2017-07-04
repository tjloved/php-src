--TEST--
Serialization of backtick literal is incorrect
--INI--
zend.assertions=1
--FILE--
<?php

function fn818420404()
{
    assert_options(ASSERT_WARNING);
    assert(false && `echo -n ""`);
}
fn818420404();
--EXPECTF--
Warning: assert(): assert(false && `echo -n ""`) failed in %s%east_serialize_backtick_literal.php on line %d

--TEST--
Misoptimization when variable is modified by assert()
--INI--
zend.assertions=1
--FILE--
<?php

function test()
{
    $i = 0;
    assert('$i = new stdClass');
    $i += 1;
    var_dump($i);
}
function fn845939789()
{
    test();
}
fn845939789();
--EXPECTF--
Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d

Notice: Object of class stdClass could not be converted to int in %s on line %d
int(2)

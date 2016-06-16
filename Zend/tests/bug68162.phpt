--TEST--
Bug #68162: isset($$varname) always true
--FILE--
<?php

function fn766005937()
{
    $name = 'var';
    var_dump(isset(${$name}));
    $var = 42;
    var_dump(isset(${$name}));
}
fn766005937();
--EXPECT--
bool(false)
bool(true)

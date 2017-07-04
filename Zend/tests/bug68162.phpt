--TEST--
Bug #68162: isset($$varname) always true
--FILE--
<?php

function fn2021006483()
{
    $name = 'var';
    var_dump(isset(${$name}));
    $var = 42;
    var_dump(isset(${$name}));
}
fn2021006483();
--EXPECT--
bool(false)
bool(true)

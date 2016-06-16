--TEST--
Bug #24399 (is_subclass_of() crashes when parent class doesn't exist)
--FILE--
<?php

class dooh
{
    public $blah;
}
function fn1183988040()
{
    $d = new dooh();
    var_dump(is_subclass_of($d, 'dooh'));
}
fn1183988040();
--EXPECT--
bool(false)

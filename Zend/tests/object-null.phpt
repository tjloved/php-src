--TEST--
Test whether an object is NULL or not.
--FILE--
<?php

class Bla
{
}
function fn1564001825()
{
    $b = new Bla();
    var_dump($b != null);
    var_dump($b == null);
    var_dump($b !== null);
    var_dump($b === null);
}
fn1564001825();
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)

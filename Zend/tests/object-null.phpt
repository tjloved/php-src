--TEST--
Test whether an object is NULL or not.
--FILE--
<?php

class Bla
{
}
function fn361250672()
{
    $b = new Bla();
    var_dump($b != null);
    var_dump($b == null);
    var_dump($b !== null);
    var_dump($b === null);
}
fn361250672();
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)

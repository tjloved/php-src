--TEST--
Bug #72629 (Caught exception assignment to variables ignores references)
--FILE--
<?php

function fn1979998810()
{
    $var = null;
    $e =& $var;
    try {
        throw new Exception();
    } catch (Exception $e) {
    }
    var_dump($var === $e);
}
fn1979998810();
--EXPECT--
bool(true)

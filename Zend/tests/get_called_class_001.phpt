--TEST--
Calling get_called_class() outside a class
--FILE--
<?php

function fn1421454493()
{
    var_dump(get_called_class());
}
fn1421454493();
--EXPECTF--
Warning: get_called_class() called from outside a class in %s on line %d
bool(false)

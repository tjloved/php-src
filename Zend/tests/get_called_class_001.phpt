--TEST--
Calling get_called_class() outside a class
--FILE--
<?php

function fn5483797()
{
    var_dump(get_called_class());
}
fn5483797();
--EXPECTF--
Warning: get_called_class() called from outside a class in %s on line %d
bool(false)

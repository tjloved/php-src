--TEST--
declare bare anonymous class
--FILE--
<?php

function fn1785408212()
{
    var_dump(new class
    {
    });
}
fn1785408212();
--EXPECTF--
object(class@%s)#%d (0) {
}



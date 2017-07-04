--TEST--
declare bare anonymous class
--FILE--
<?php

function fn214135666()
{
    var_dump(new class
    {
    });
}
fn214135666();
--EXPECTF--
object(class@%s)#%d (0) {
}



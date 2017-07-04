--TEST--
Bug #72523 (dtrace issue with reflection (failed test))
--FILE--
<?php

function fn905395555()
{
    $gen = (new class
    {
        function a()
        {
            (yield "okey");
        }
    })->a();
    var_dump($gen->current());
}
fn905395555();
--EXPECT--
string(4) "okey"

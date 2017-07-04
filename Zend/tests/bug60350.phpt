--TEST--
Bug #60350 No string escape code for ESC (ascii 27), normally \e
--FILE--
<?php

function fn509937169()
{
    $str = "\33";
    if (ord($str) == 27) {
        echo "Works";
    }
}
fn509937169();
--EXPECT--
Works

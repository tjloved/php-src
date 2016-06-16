--TEST--
Bug #60350 No string escape code for ESC (ascii 27), normally \e
--FILE--
<?php

function fn1252602365()
{
    $str = "";
    if (ord($str) == 27) {
        echo "Works";
    }
}
fn1252602365();
--EXPECT--
Works

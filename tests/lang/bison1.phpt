--TEST--
Bison weirdness
--FILE--
<?php

function fn1256944790()
{
    error_reporting(E_ALL & ~E_NOTICE);
    echo "blah-{$foo}\n";
}
fn1256944790();
--EXPECT--
blah-

--TEST--
Bison weirdness
--FILE--
<?php

function fn1651245518()
{
    error_reporting(E_ALL & ~E_NOTICE);
    echo "blah-{$foo}\n";
}
fn1651245518();
--EXPECT--
blah-

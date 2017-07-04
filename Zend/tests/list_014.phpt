--TEST--
Cannot destructure using array(), even if nested
--FILE--
<?php

function fn1630697708()
{
    [array($a)] = [array(42)];
    var_dump($a);
}
fn1630697708();
--EXPECTF--
Fatal error: Cannot assign to array(), use [] instead in %s on line %d

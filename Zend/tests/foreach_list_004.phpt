--TEST--
foreach with empty list
--FILE--
<?php

function fn103996675()
{
    $array = [['a', 'b'], 'c', 'd'];
    foreach ($array as $key => list()) {
    }
}
fn103996675();
--EXPECTF--
Fatal error: Cannot use empty list in %sforeach_list_004.php on line %d

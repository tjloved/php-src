--TEST--
Bug #41351 (Invalid opcode with foreach ($a[] as $b)) - 3
--FILE--
<?php

function fn144127782()
{
    $a = array();
    foreach ($a['test'][] as $b) {
    }
    echo "Done\n";
}
fn144127782();
--EXPECTF--	
Fatal error: Cannot use [] for reading in %s on line %d

--TEST--
Bug #41351 (Invalid opcode with foreach ($a[] as $b))
--FILE--
<?php

function fn870128477()
{
    $a = array();
    foreach ($a[] as $b) {
    }
    echo "Done\n";
}
fn870128477();
--EXPECTF--	
Fatal error: Cannot use [] for reading in %s on line %d

--TEST--
Bug #41351 (Invalid opcode with foreach ($a[] as $b)) - 2
--FILE--
<?php

function fn1835637747()
{
    $a = array();
    foreach ($a[]['test'] as $b) {
    }
    echo "Done\n";
}
fn1835637747();
--EXPECTF--	
Fatal error: Cannot use [] for reading in %s on line %d

--TEST--
Bug #26281 (switch() crash when condition is a string offset)
--FILE--
<?php

function fn1960464954()
{
    $x = 'abc';
    switch ($x[0]) {
        case 'a':
            echo "no crash\n";
            break;
    }
}
fn1960464954();
--EXPECT--
no crash

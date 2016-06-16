--TEST--
Bug #26281 (switch() crash when condition is a string offset)
--FILE--
<?php

function fn777135778()
{
    $x = 'abc';
    switch ($x[0]) {
        case 'a':
            echo "no crash\n";
            break;
    }
}
fn777135778();
--EXPECT--
no crash

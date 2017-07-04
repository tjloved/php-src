--TEST--
Bug #26696 (string index in a switch() crashes with multiple matches)
--FILE--
<?php

function fn616795629()
{
    $str = 'asdd/?';
    $len = strlen($str);
    for ($i = 0; $i < $len; $i++) {
        switch ($str[$i]) {
            case '?':
                echo "OK\n";
                break;
        }
    }
    $str = '*';
    switch ($str[0]) {
        case '*':
            echo "OK\n";
            break;
        default:
            echo 'Default RAN!';
    }
}
fn616795629();
--EXPECT--
OK
OK

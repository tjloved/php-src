--TEST--
Accept hashes being equal to zero
--FILE--
<?php

function fn891131813()
{
    $hashes = ["Ž\32c\17a" => 32, "÷\27k\3j\23c\27k\35g" => 64];
    foreach ($hashes as $hash => $bits) {
        var_dump($hashes[$hash], $bits);
    }
}
fn891131813();
--EXPECT--
int(32)
int(32)
int(64)
int(64)

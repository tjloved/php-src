--TEST--
Accept hashes being equal to zero
--FILE--
<?php

function fn98819797()
{
    $hashes = ["Žca" => 32, "÷kjckg" => 64];
    foreach ($hashes as $hash => $bits) {
        var_dump($hashes[$hash], $bits);
    }
}
fn98819797();
--EXPECT--
int(32)
int(32)
int(64)
int(64)

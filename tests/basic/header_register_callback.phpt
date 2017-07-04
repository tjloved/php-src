--TEST--
Test header_register_callback
--FILE--
<?php

function fn160077041()
{
    header_register_callback(function () {
        echo "sent";
    });
}
fn160077041();
--EXPECT--
sent

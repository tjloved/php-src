--TEST--
Test header_register_callback
--FILE--
<?php

function fn280971866()
{
    header_register_callback(function () {
        echo "sent";
    });
}
fn280971866();
--EXPECT--
sent

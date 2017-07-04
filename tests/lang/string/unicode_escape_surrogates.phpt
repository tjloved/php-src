--TEST--
Valid Unicode escape sequences: Surrogate halves
--FILE--
<?php

function fn438393125()
{
    // Surrogate pairs are non-well-formed UTF-8 - however, it is sometimes useful
    // to be able to produce these (e.g. CESU-8 handling)
    var_dump(bin2hex("í "));
    var_dump(bin2hex("í°€"));
    var_dump(bin2hex("í í°€"));
    // CESU-8 encoding of U+10400
}
fn438393125();
--EXPECT--
string(6) "eda081"
string(6) "edb080"
string(12) "eda081edb080"

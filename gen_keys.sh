#!/bin/bash

/opt/cprocsp/bin/amd64/csptest -keyset -newkeyset -silent -cont '\\.\HDIMAGE\ca_keyset'

/opt/cprocsp/bin/amd64/csptest -keyset -newkeyset -silent -cont '\\.\HDIMAGE\csr_keyset'

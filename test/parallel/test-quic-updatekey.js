// Flags: --no-warnings
'use strict';

// Test QuicSocket constructor option errors.

const common = require('../common');
if (!common.hasQuic)
  common.skip('missing quic');

const assert = require('assert');
const Countdown = require('../common/countdown');
const fixtures = require('../common/fixtures');
const key = fixtures.readKey('agent1-key.pem', 'binary');
const cert = fixtures.readKey('agent1-cert.pem', 'binary');
const ca = fixtures.readKey('ca1-cert.pem', 'binary');

const { createSocket } = require('quic');
const kServerPort = process.env.NODE_DEBUG_KEYLOG ? 5678 : 0;
const kClientPort = process.env.NODE_DEBUG_KEYLOG ? 5679 : 0;
let client;

const server = createSocket({ port: kServerPort, validateAddress: true });
server.setDiagnosticPacketLoss({ rx: 0.00, tx: 0.00 });
const kALPN = 'meow';  // ALPN can be overriden to whatever we want
const kServerName = 'agent2';  // Intentionally the wrong servername


const countdown = new Countdown(2, () => {
  server.close();
  client.close();
});

server.listen({
  key,
  cert,
  ca,
  requestCert: true,
  rejectUnauthorized: false,
  alpn: kALPN,
  maxCryptoBuffer: 4096,
});
server.on('session', common.mustCall((session) => {
  process.nextTick(() => {
    assert.throws(() => session.updateKey(), {
      code: 'ERR_QUICSESSION_UPDATEKEY',
      name: 'Error',
      message: 'Unable to update QuicSession keys'
    });
  });
}));

server.on('ready', common.mustCall(() => {
  client = createSocket({
    port: kClientPort,
    client: {
      key,
      cert,
      ca,
      alpn: kALPN,
    }
  });

  client.connect({
    address: 'localhost',
    port: server.address.port,
    servername: kServerName,
    requestOCSP: true,
  });
  
}));



/*!
    \fn MCPD8::init(void)
 */
bool MCPD8p::init(void)
{
// set tx capability to P
	initCmdBuffer(WRITEREGISTER);
	m_cmdBuf.data[0] = 1;
	m_cmdBuf.data[1] = 103;
	m_cmdBuf.data[2] = 1;
	finishCmdBuffer(3);
	return sendCommand();
}

        // now set tx capabilities, if id == 105
        if(myMpsd[id]->getMpsdId() == 105){
                myMpsd[id]->setInternalreg(1, 4, 1);
                commandBuffer[1] = WRITEPERIREG;
                commandBuffer[2] = id;
                commandBuffer[3] = 1;   // write register 1
                commandBuffer[4] = 4;
                sendCommand(commandBuffer);
        }


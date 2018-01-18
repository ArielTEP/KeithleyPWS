""" Este modulo se comunica con la fuente Keysight """
import visa

resourceManager = visa.ResourceManager() # pylint: disable=invalid-name
print 'List of resources: ' + str(resourceManager.list_resources()) 
print resourceManager
# u'USB0::0x05E6::0x2200::1358231::INSTR'
try:
    pws = resourceManager.open_resource(u'USB0::0x05E6::0x2200::1358231::INSTR')
    print pws.query('*IDN?')
except BaseException as e:
    print 'El recurso no fue encontrado, verifique que la fuente este conectada.'
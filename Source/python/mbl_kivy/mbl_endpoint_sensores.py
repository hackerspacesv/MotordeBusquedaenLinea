from mbl_layout_endpoint import LayoutEndpoint

class LayoutEndpointSensores(LayoutEndpoint):
  def __init__(self, i32ctt, direccion_mac, endpoint, **kwargs):
    super(LayoutEndpointSensores, self).__init__(i32ctt, direccion_mac, endpoint, **kwargs)
